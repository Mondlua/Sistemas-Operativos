#include "alg_planificacion.h"

void* hilo_planificador(void *args)
{
    t_planificacion *kernel_argumentos = (t_planificacion*) args;

    t_tipo_planificacion algoritmo_planificador = obtener_algoritmo_planificador(kernel_argumentos->config.algoritmo_planificador);
    log_debug(kernel_argumentos->logger, "Algoritmo planificador: %d", algoritmo_planificador);
    kernel_argumentos->algo_planning = algoritmo_planificador;
    while(1)
    {
        // Espero a que me soliciten planificar
        sem_wait(&kernel_argumentos->planificar);
        log_debug(kernel_argumentos->logger, "Planificando...");

        // Si se ejecuto DETENER_PLANIFICACION, no planifico por mas que me soliciten hacerlo
        if(kernel_argumentos->detener_planificacion)
        {
            log_debug(kernel_argumentos->logger, "Planificacion detenida.");
            continue;
        }

        // Planifico a largo plazo
        planificador_largo_plazo(kernel_argumentos);

        // Planifico a corto plazo
        planificador_corto_plazo(algoritmo_planificador, kernel_argumentos);
    }
    return NULL;
}

void fifo(t_planificacion *kernel_argumentos)
{
    if(queue_size(kernel_argumentos->colas.exec) == 0 && queue_size(kernel_argumentos->colas.ready) >= 1)
    {
        t_pcb *proximo_pcb = planificador_ready_a_exec(kernel_argumentos);
        enviar_pcb(proximo_pcb, kernel_argumentos->socket_cpu_dispatch);
    }
    return;
}

void round_robin(t_planificacion *kernel_argumentos)
{
    if(queue_size(kernel_argumentos->colas.exec) == 0 && queue_size(kernel_argumentos->colas.ready) >= 1)
    {
        t_pcb* proximo_pcb = planificador_ready_a_exec(kernel_argumentos);
        iniciar_timer(kernel_argumentos->timer_quantum, kernel_argumentos->config.quantum);
        enviar_pcb(proximo_pcb, kernel_argumentos->socket_cpu_dispatch);
    }
    return;
}

void virtual_round_robin(t_planificacion *kernel_argumentos)
{
    if(queue_size(kernel_argumentos->colas.exec) == 0 && queue_size(kernel_argumentos->colas.prioridad) >= 1)
    {
        t_pcb *proximo_pcb = planificador_prioridad_a_exec(kernel_argumentos);

        int quantum_restante = kernel_argumentos->config.quantum - proximo_pcb->quantum;
        iniciar_timer(kernel_argumentos->timer_quantum, quantum_restante);
        return;
    }
    if(queue_size(kernel_argumentos->colas.exec) == 0 && queue_size(kernel_argumentos->colas.ready) >= 1)
    {
        t_pcb *proximo_pcb = planificador_ready_a_exec(kernel_argumentos);
        iniciar_timer(kernel_argumentos->timer_quantum, kernel_argumentos->config.quantum);

        enviar_pcb(proximo_pcb, kernel_argumentos->socket_cpu_dispatch);
    }
    return;
}

void iniciar_timer(t_timer_planificador* timer, int milisegundos)
{
    timer->its.it_value.tv_sec = milisegundos / 1000;
    timer->its.it_value.tv_nsec = (milisegundos % 1000) * 1000000;
    timer->its.it_interval.tv_nsec = 0;
    timer->its.it_interval.tv_sec = 0;

    timer_settime(timer->timer, 0, &timer->its, NULL);
}

int frenar_timer(t_timer_planificador* timer)
{
    struct itimerspec remaining;
    timer_gettime(timer->timer, &remaining);
    //timer_delete(timer->timer);
    return remaining.it_value.tv_nsec * 1000;
}

void enviar_interrupcion(union sigval sv)
{
    t_planificacion *kernel_argumentos = (t_planificacion*)sv.sival_ptr;
    log_warning(kernel_argumentos->logger, "En este momento deberia mandar la interrupcion");
    // Enviar interrupcion a CPU Interrupt
}

void planificador_planificar(t_planificacion *kernel_argumentos)
{
    sem_post(&kernel_argumentos->planificar);
}

bool planificador_recepcion_pcb(t_pcb *pcb_desalojado, t_planificacion *kernel_argumentos)
{
    // Saco el pcb viejo de EXEC
    t_pcb *pcb_outdated = queue_pop(kernel_argumentos->colas.exec);
    log_debug(kernel_argumentos->logger, "PID recibido: %d", pcb_desalojado->pid);
    log_debug(kernel_argumentos->logger, "PID que tengo en EXEC: %d", pcb_outdated->pid);

    if(pcb_desalojado->pid != pcb_outdated->pid)
    {   
        log_error(kernel_argumentos->logger, "Discordancia entre el pcb en EXEC y el ejecutado por CPU.");
        return true;
    }

    // Meto el pcb a la cola correspondiente
    if(pcb_desalojado->motivo_desalojo == 0) // Desalojado por haber ejecutado EXIT
    {
        // Mato al timer si corresponde
        if(kernel_argumentos->algo_planning != FIFO)
        {
            frenar_timer(kernel_argumentos->timer_quantum);
        }

        queue_push(kernel_argumentos->colas.exit, pcb_desalojado);
        // Enviar solicitud a memoria para desalojar el proceso
        log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: EXEC - Estado actual: EXIT", pcb_desalojado->pid);
        log_info(kernel_argumentos->logger, "Finaliza el proceso %d - Motivo: SUCCESS", pcb_desalojado->pid);
    }
    if(pcb_desalojado->motivo_desalojo == 1) // Desalojado por fin de quantum
    {
        queue_push(kernel_argumentos->colas.ready, pcb_desalojado);
        log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: EXEC - Estado actual: READY", pcb_desalojado->pid);
    }
    if(pcb_desalojado->motivo_desalojo == 2) // Desalojado por IO_BLOCK
    {
        int milisegundos_restantes = frenar_timer(kernel_argumentos->timer_quantum);
        pcb_desalojado->quantum = milisegundos_restantes;
        // mover_a_block(kernel_argumentos, pcb_desalojado, nombre_interfaz);
        log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: EXEC - Estado actual: BLOCK", pcb_desalojado->pid);
        // Realizar la solicitud correspondiente a IO
    }
    if(pcb_desalojado->motivo_desalojo == 3) // Desalojado por WAIT
    {
        char* recurso_solicitado = "placeholder";
        return administrador_recursos_wait(pcb_desalojado, recurso_solicitado, kernel_argumentos);
        // Verificar existencia y disponibilidad del recurso solicitado
        // Si no existe, a EXIT y return true.
        // Si existe, pero no hay instancias, a BLOCK correspondiente y return true
        // Si existe y hay instancias, se lo devuelve a EXEC, y se retorna un valor para que no se replanifique
    }
    if(pcb_desalojado->motivo_desalojo == 4) // Desalojado por EXIT
    {
        char* recurso_solicitado = "placeholder";
        administrador_recursos_signal(pcb_desalojado, recurso_solicitado, kernel_argumentos);
        // Verificar existencia del recurso solicitado
        // Si no existe, a EXIT y return true
        // Si existe, se le suma 1 al indice correspondiente y se lo devuelve a EXEC.
        // Si hay procesos esperando, se mueve uno de BLOCK a READY
        return false;
    }
    return true;
}

void planificador_corto_plazo(t_tipo_planificacion algoritmo, t_planificacion *kernel_argumentos)
{
    switch(algoritmo)
    {
        case FIFO:
        {
            fifo(kernel_argumentos);
        }
        case RR:
        {
            round_robin(kernel_argumentos);
        }
        case VRR:
        {
            virtual_round_robin(kernel_argumentos);
        }
    }
}

void planificador_largo_plazo(t_planificacion *kernel_argumentos)
{
    int cantidad_procesos_actual = queue_size(kernel_argumentos->colas.exec);
    cantidad_procesos_actual += queue_size(kernel_argumentos->colas.ready);
    cantidad_procesos_actual += kernel_argumentos->colas.cantidad_procesos_block;
    cantidad_procesos_actual += queue_size(kernel_argumentos->colas.prioridad);

    if(cantidad_procesos_actual < kernel_argumentos->config.grado_multiprogramacion)
    {
        while(cantidad_procesos_actual < kernel_argumentos->config.grado_multiprogramacion && !queue_is_empty(kernel_argumentos->colas.new))
        {
            t_pcb* pcb = queue_pop(kernel_argumentos->colas.new);
            queue_push(kernel_argumentos->colas.ready, pcb);
            log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: NEW - Estado actual: READY", pcb->pid);
            cantidad_procesos_actual++;
        }
        if(queue_is_empty(kernel_argumentos->colas.new))
        {
            //log_warning(kernel_argumentos->logger, "No hay mas argumentos en NEW para pasar a READY.");
        }
    }
}

t_planificacion *inicializar_t_planificacion(t_config *kernel_config, t_log *kernel_log)
{
   t_planificacion *planificador = malloc(sizeof(t_planificacion));
    
    planificador->logger = kernel_log;

    planificador->colas.new = queue_create();
    planificador->colas.ready = queue_create();
    planificador->colas.exec = queue_create();
    planificador->colas.exit = queue_create();
    planificador->colas.prioridad = queue_create();
    planificador->colas.lista_block = dictionary_create();

    planificador->detener_planificacion = 0;
    planificador->colas.cantidad_procesos_block = 0;

    sem_init(&planificador->planificar, 0, 0);

    inicializar_lista_recursos(planificador, kernel_config);

    inicializar_config_kernel(planificador, kernel_config);

    planificador->timer_quantum = malloc(sizeof(t_timer_planificador));
    planificador->timer_quantum->timer = inicializar_timer(planificador);

    return planificador;
}

timer_t inicializar_timer(t_planificacion *kernel_argumentos)
{
    struct sigevent sev;
    timer_t ret;

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_value.sival_ptr = kernel_argumentos;
    sev.sigev_notify_function = enviar_interrupcion;
    sev.sigev_notify_attributes = NULL;

    timer_create(CLOCK_REALTIME, &sev, &ret);

    return ret;
}

t_tipo_planificacion obtener_algoritmo_planificador(char* algoritmo)
{
    if(strcmp(algoritmo, "FIFO") == 0)
    {
        return FIFO;
    }
    if(strcmp(algoritmo, "RR") == 0)
    {
        return RR;
    }
    if(strcmp(algoritmo, "VRR") == 0)
    {
        return VRR;
    }
    
    // Se elige fifo por default
    return FIFO;
}

void inicializar_lista_recursos(t_planificacion *planificador, t_config *kernel_config)
{
    char* string_arr_recursos = config_get_string_value(kernel_config, "RECURSOS");
    char* string_arr_instancias_recursos = config_get_string_value(kernel_config, "INSTANCIAS_RECURSOS");

    char** array_nombre_recursos = string_get_string_as_array(string_arr_recursos);
    char** array_instancias_recursos = string_get_string_as_array(string_arr_instancias_recursos);

    int i = 0;
    while(array_nombre_recursos[i] != NULL)
    {
        t_queue_block *block_queue = malloc(sizeof(t_queue_block));
        block_queue->block_queue = queue_create();
        block_queue->identificador = string_duplicate(array_nombre_recursos[i]);
        block_queue->cantidad_instancias = atoi(array_instancias_recursos[i]);
        block_queue->socket_interfaz = 0;
        printf("Recurso agregado\n");

        dictionary_put(planificador->colas.lista_block, block_queue->identificador, block_queue);
        free(block_queue); // Este free capaz no tenga que estar aca, sino al final de la ejecucion
        i++;
    }

    //free(array_nombre_recursos);
    //free(array_instancias_recursos);
}

void inicializar_config_kernel(t_planificacion *planificador, t_config *kernel_config)
{
    planificador->config.algoritmo_planificador = config_get_string_value(kernel_config, "ALGORITMO_PLANIFICACION");
    planificador->config.grado_multiprogramacion = config_get_int_value(kernel_config, "GRADO_MULTIPROGRAMACION");
    planificador->config.quantum = config_get_int_value(kernel_config, "QUANTUM");
    planificador->config.config_leida.ip_cpu = config_get_string_value(kernel_config, "IP_CPU");
    planificador->config.config_leida.ip_memoria = config_get_string_value(kernel_config, "IP_MEMORIA");
    planificador->config.config_leida.puerto_cpu_dispatch = config_get_string_value(kernel_config, "PUERTO_CPU_DISPATCH");
    planificador->config.config_leida.puerto_cpu_interrupt = config_get_string_value(kernel_config, "PUERTO_CPU_INTERRUPT");
    planificador->config.config_leida.puerto_escucha = config_get_int_value(kernel_config, "PUERTO_ESCUCHA");
    planificador->config.config_leida.puerto_memoria = config_get_int_value(kernel_config, "PUERTO_MEMORIA");
}
// ------- TRANSICIONES DE ESTADOS --------
t_pcb *planificador_ready_a_exec(t_planificacion *kernel_argumentos)
{
    
    t_pcb *proximo_proceso = queue_pop(kernel_argumentos->colas.ready);
    proximo_proceso->estado = EXEC;
    queue_push(kernel_argumentos->colas.exec, proximo_proceso);
    log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: READY - Estado actual: EXEC", proximo_proceso->pid);
    return proximo_proceso;
}

t_pcb *planificador_prioridad_a_exec(t_planificacion *kernel_argumentos)
{
    t_pcb *proximo_pcb = queue_pop(kernel_argumentos->colas.prioridad);
    proximo_pcb->estado = EXEC;
    queue_push(kernel_argumentos->colas.exec, proximo_pcb);
    log_info(kernel_argumentos->logger, "PID %d - Estado anterior: READY - Estado actual: EXEC", proximo_pcb->pid);
    return proximo_pcb;
}

// -------- ADMINISTRACION DE RECURSOS --------

bool administrador_recursos_wait(t_pcb *pcb_solicitante, char* nombre_recurso, t_planificacion *kernel_argumentos)
{
    t_queue_block *recurso = dictionary_get(kernel_argumentos->colas.lista_block, nombre_recurso);
    
    if(recurso == NULL)
    {
        // El recurso no existe. Mando el proceso a EXIT y habilito la replanificacion
        // TODO: cambiar esto a una funcion que remueva de la lista de bloqueados al proceso solicitante, no al primero
        queue_pop(recurso->block_queue); 
        queue_push(kernel_argumentos->colas.exit, pcb_solicitante);
        log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: EXEC - Estado Actual: EXIT", pcb_solicitante->pid);
        log_info(kernel_argumentos->logger, "Finaliza el proceso %d - Motivo: INVALID_RESOURCE", pcb_solicitante->pid);
        return true;
    }

    recurso->cantidad_instancias--;

    if(recurso->cantidad_instancias >= 0)
    {
        // Se habilita la instancia del recurso para el proceso
        queue_push(kernel_argumentos->colas.exec, pcb_solicitante);
        log_debug(kernel_argumentos->logger, "Se devuelve el PID: %d a cpu por haber solicitado un recurso disponible.", pcb_solicitante->pid);
        enviar_pcb(pcb_solicitante, kernel_argumentos->socket_cpu_dispatch);
        return false;
    }

    // Se bloque el proceso
    queue_push(recurso->block_queue, pcb_solicitante);
    log_info(kernel_argumentos->logger, "PID: %d - Bloqueado por: %s", pcb_solicitante->pid, nombre_recurso);
    return true;

}

bool administrador_recursos_signal(t_pcb *pcb_desalojado, char* recurso_solicitado, t_planificacion *kernel_argumentos)
{
    t_queue_block *recurso = dictionary_get(kernel_argumentos->colas.lista_block, recurso_solicitado);

    if(recurso == NULL)
    {
        // El recurso no existe. Mando el proceso a EXIT y habilito la replanificacion
        // TODO: cambiar esto a una funcion que remueva de la lista de bloqueados al proceso solicitante, no al primero
        queue_pop(recurso->block_queue); 
        queue_push(kernel_argumentos->colas.exit, pcb_desalojado);
        log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: EXEC - Estado Actual: EXIT", pcb_desalojado->pid);
        log_info(kernel_argumentos->logger, "Finaliza el proceso %d - Motivo: INVALID_RESOURCE", pcb_desalojado->pid);
        return true;
    }

    recurso->cantidad_instancias++;

    procesar_desbloqueo_factible(recurso_solicitado, kernel_argumentos);

    // Devuelvo el pcb a CPU
    enviar_pcb(pcb_desalojado, kernel_argumentos->socket_cpu_dispatch);
    return false;
}

void procesar_desbloqueo_factible(char* recurso_solicitado, t_planificacion *kernel_argumentos)
{
    t_queue_block *recurso = dictionary_get(kernel_argumentos->colas.lista_block, recurso_solicitado);

    if(!queue_is_empty(recurso->block_queue))
    {
        t_pcb *pcb_desbloqueado = queue_pop(recurso->block_queue);
        queue_push(kernel_argumentos->colas.ready, pcb_desbloqueado);
        recurso->cantidad_instancias--;
        log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: BLOCK - Estado actual: READY", pcb_desbloqueado->pid);
    }
}