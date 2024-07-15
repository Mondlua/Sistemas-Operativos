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
        pthread_mutex_unlock(&kernel_argumentos->planning_mutex);
    }
    return NULL;
}

void fifo(t_planificacion *kernel_argumentos)
{
    if(queue_size(kernel_argumentos->colas.exec) == 0 && queue_size(kernel_argumentos->colas.ready) >= 1)
    {
        t_pcb *proximo_pcb = planificador_ready_a_exec(kernel_argumentos);
        proximo_pcb->estado = EXEC;
        enviar_pcb(proximo_pcb, kernel_argumentos->socket_cpu_dispatch);
    }
    return;
}

void round_robin(t_planificacion *kernel_argumentos)
{
    if(queue_size(kernel_argumentos->colas.exec) == 0 && queue_size(kernel_argumentos->colas.ready) >= 1)
    {
        t_pcb* proximo_pcb = planificador_ready_a_exec(kernel_argumentos);
        proximo_pcb->estado = EXEC;
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
        proximo_pcb->estado = EXEC;

        int quantum_restante = kernel_argumentos->config.quantum - proximo_pcb->quantum;
        iniciar_timer(kernel_argumentos->timer_quantum, quantum_restante);

        enviar_pcb(proximo_pcb, kernel_argumentos->socket_cpu_dispatch);
        return;
    }
    if(queue_size(kernel_argumentos->colas.exec) == 0 && queue_size(kernel_argumentos->colas.ready) >= 1)
    {
        t_pcb *proximo_pcb = planificador_ready_a_exec(kernel_argumentos);
        proximo_pcb->estado = EXEC;

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
    return remaining.it_value.tv_nsec / 1000000;
}

void enviar_interrupcion(union sigval sv)
{
    t_planificacion *kernel_argumentos = (t_planificacion*)sv.sival_ptr;

    // Enviar interrupcion a CPU Interrupt
    if(!queue_is_empty(kernel_argumentos->colas.exec))
    {
        t_pcb* pcb_actual = queue_peek(kernel_argumentos->colas.exec);
        enviar_int_a_interrupt(kernel_argumentos->socket_cpu_interrupt, pcb_actual->pid);
        log_debug(kernel_argumentos->logger, "Interrupcion al PID: %d enviada.", pcb_actual->pid);
        return;
    }
    
    log_debug(kernel_argumentos->logger, "");
}

void planificador_planificar(t_planificacion *kernel_argumentos)
{
    sem_post(&kernel_argumentos->planificar);
}

bool planificador_recepcion_pcb(t_pcb *pcb_desalojado, t_planificacion *kernel_argumentos)
{
    // Saco el pcb viejo de EXEC
    pthread_mutex_lock(&kernel_argumentos->planning_mutex);
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

        mover_a_exit(pcb_desalojado, kernel_argumentos);
        //pcb_desalojado->estado = EXIT;
        //queue_push(kernel_argumentos->colas.exit, pcb_desalojado);
        // Enviar solicitud a memoria para desalojar el proceso
        //log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: EXEC - Estado actual: EXIT", pcb_desalojado->pid);
        log_info(kernel_argumentos->logger, "Finaliza el proceso %d - Motivo: SUCCESS", pcb_desalojado->pid);
    }
    if(pcb_desalojado->motivo_desalojo == 1) // Desalojado por fin de quantum
    {
        pcb_desalojado->estado = READY;
        queue_push(kernel_argumentos->colas.ready, pcb_desalojado);
        log_info(kernel_argumentos->logger, "PID: %d - Desalojado por fin de Quantum", pcb_desalojado->pid);
        log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: EXEC - Estado actual: READY", pcb_desalojado->pid);
    }
    if(pcb_desalojado->motivo_desalojo == 2) // Desalojado por IO_BLOCK
    {
        int milisegundos_restantes = frenar_timer(kernel_argumentos->timer_quantum);
        if(kernel_argumentos->algo_planning == VRR)
        {
            pcb_desalojado->quantum = milisegundos_restantes;
            log_debug(kernel_argumentos->logger, "Milisegundos restantes: %d", pcb_desalojado->quantum);
        }
        // Recibir los parametros del io_block
        t_instruccion_params_opcode parametros_solicitud;
        parametros_solicitud = recibir_solicitud_cpu(kernel_argumentos->socket_cpu_dispatch, pcb_desalojado);
        log_debug(kernel_argumentos->logger, "Solicitud recibida: %s, %d", parametros_solicitud.params->interfaz, parametros_solicitud.params->params.io_gen_sleep_params.unidades_trabajo);
        validar_peticion(parametros_solicitud.params, pcb_desalojado, parametros_solicitud.opcode, kernel_argumentos);
        // mover_a_block(kernel_argumentos, pcb_desalojado, nombre_interfaz);
        //log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: EXEC - Estado actual: BLOCK", pcb_desalojado->pid);
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
    if(pcb_desalojado->motivo_desalojo == 4) // Desalojado por SIGNAL
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
        if(queue_is_empty(kernel_argumentos->colas.new))
        {
            return;
        }
        while(cantidad_procesos_actual < kernel_argumentos->config.grado_multiprogramacion && !queue_is_empty(kernel_argumentos->colas.new))
        {
            t_pcb* pcb = queue_pop(kernel_argumentos->colas.new);
            queue_push(kernel_argumentos->colas.ready, pcb);
            log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: NEW - Estado actual: READY", pcb->pid);
            cantidad_procesos_actual++;
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

    pthread_mutex_init(&planificador->planning_mutex, NULL);
    pthread_mutex_init(&planificador->colas.mutex_block, NULL);
    pthread_mutex_init(&planificador->colas.mutex_exec, NULL);
    pthread_mutex_init(&planificador->colas.mutex_exit, NULL);
    pthread_mutex_init(&planificador->colas.mutex_new, NULL);
    pthread_mutex_init(&planificador->colas.mutex_prioridad, NULL);
    pthread_mutex_init(&planificador->colas.mutex_ready, NULL);

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

// -------- ADMINISTRACION DE INTERFACES --------

void validar_peticion(instruccion_params* parametros, t_pcb* pcb, int codigo_op, t_planificacion* kernel_argumentos) {

    if(dictionary_is_empty(kernel_argumentos->colas.lista_block))
    {
        log_warning(kernel_argumentos->logger, "No hay interfaces concetadas.");
        pcb->estado = EXIT;
        queue_push(kernel_argumentos->colas.exit, pcb);
        log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: EXEC - Estado actual: EXIT", pcb->pid);
        log_info(kernel_argumentos->logger, "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", pcb->pid);
        return;
    }

    t_queue_block *interfaz_solicitada = dictionary_get(kernel_argumentos->colas.lista_block, parametros->interfaz);
    if(interfaz_solicitada == NULL)
    {
        pcb->estado = EXIT;
        queue_push(kernel_argumentos->colas.exit, pcb);
        log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: EXEC - Estado actual: EXIT", pcb->pid);
        log_info(kernel_argumentos->logger, "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", pcb->pid);
        return;
    }

    enviar_instruccion_a_interfaz(interfaz_solicitada, parametros, codigo_op);
    log_debug(kernel_argumentos->logger, "Instruccion solicitada a la interfaz: %s", interfaz_solicitada->identificador);
    log_debug(kernel_argumentos->logger, "Enviada por el socket: %d", interfaz_solicitada->socket_interfaz);

    pcb->estado = BLOCKED;
    queue_push(interfaz_solicitada->block_queue, pcb);
    kernel_argumentos->colas.cantidad_procesos_block++;
    log_info(kernel_argumentos->logger, "PID: %d - Bloqueado por: INTERFAZ", pcb->pid);
    log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: EXEC - Estado actual: BLOCK", pcb->pid);

}

interfaz* buscar_interfaz_por_nombre(char* nombre_interfaz) {
    // int tamanio_lista = list_size(interfaces);
    // interfaz* interfaz_encontrada = NULL;

    // for (int i = 0; i < tamanio_lista; i++) {
    //     interfaz* posible_interfaz = (interfaz*)list_get(interfaces, i);
    //     if (string_equals_ignore_case(posible_interfaz->nombre_interfaz, nombre_interfaz)) {
    //         interfaz_encontrada = posible_interfaz;
    //         break;
    //     }
    // }
    // return interfaz_encontrada;
}

void enviar_instruccion_a_interfaz(t_queue_block* interfaz_destino, instruccion_params* parametros, int codigo_op) {
    t_paquete_instruccion* instruccion_enviar = malloc(sizeof(t_paquete_instruccion));

    instruccion_enviar->codigo_operacion = codigo_op;
    enviar_instruccion(instruccion_enviar, parametros, interfaz_destino->socket_interfaz);

    free(parametros);
    free(instruccion_enviar);
}

instruccion_params* deserializar_io_gen_sleep_con_interfaz(t_buffer_ins* buffer)
{
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    
    uint32_t offset = 0;
    uint32_t interfaz_len;
    memcpy(&interfaz_len, buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    parametros->interfaz = malloc(interfaz_len);
    memcpy(parametros->interfaz, buffer->stream + offset, interfaz_len);
    offset += interfaz_len;
    memcpy(&(parametros->params.io_gen_sleep_params.unidades_trabajo), buffer->stream + offset, sizeof(int));
    
    return parametros;
}

instruccion_params* deserializar_io_stdin_stdout_con_interfaz(t_buffer_ins* buffer)
{
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    
    uint32_t offset = 0;
    uint32_t interfaz_len;
    memcpy(&interfaz_len, buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    parametros->interfaz = malloc(interfaz_len);
    memcpy(parametros->interfaz, buffer->stream + offset, interfaz_len);
    offset += interfaz_len;
    memcpy(&(parametros->params.io_stdin_stdout.registro_direccion), buffer->stream + offset, sizeof(t_dir_fisica));
    offset += sizeof(t_dir_fisica);
    memcpy(&(parametros->params.io_stdin_stdout.registro_tamaño), buffer->stream + offset, sizeof(cpu_registros));
    return parametros;
}

t_instruccion_params_opcode recibir_solicitud_cpu(int socket_servidor, t_pcb* pcb)
{
    t_paquete_instruccion* instruccion = malloc(sizeof(t_paquete_instruccion));
    instruccion->buffer = malloc(sizeof(t_buffer_ins));
    
    printf("Antes de los recv\n");
    recv(socket_servidor, &(instruccion->codigo_operacion), sizeof(instrucciones), MSG_WAITALL);
    recv(socket_servidor, &(instruccion->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    printf("Recibido!\n");
    
    instruccion->buffer->stream = malloc(instruccion->buffer->size);
    
    recv(socket_servidor, instruccion->buffer->stream, instruccion->buffer->size, MSG_WAITALL);
    instruccion_params* param = NULL;
   
    switch (instruccion->codigo_operacion) {
        case IO_GEN_SLEEP:{
            param = deserializar_io_gen_sleep_con_interfaz(instruccion->buffer);
            break;
        }
        case IO_STDIN_READ:{
            param = deserializar_io_stdin_stdout_con_interfaz(instruccion->buffer);
            break;
        }
        case IO_STDOUT_WRITE:{
            param = deserializar_io_stdin_stdout_con_interfaz(instruccion->buffer);
            break;
        }
         // Otros casos
        default:
            printf("Tipo de operación no válido.\n");
            break;
        }

    t_instruccion_params_opcode ret;
    ret.opcode = instruccion->codigo_operacion;
    ret.params = param;

    free(instruccion->buffer->stream);
    free(instruccion->buffer);
    free(instruccion);

    return ret;
}

void pcb_a_exit_por_sol_invalida(t_queue_block* interfaz, t_planificacion* kernel_argumentos)
{
    pthread_mutex_lock(&kernel_argumentos->colas.mutex_block);
    t_pcb *pcb_desalojado = queue_pop(interfaz->block_queue);
    pthread_mutex_unlock(&kernel_argumentos->colas.mutex_block);

    mover_a_exit(pcb_desalojado, kernel_argumentos);

    log_info(kernel_argumentos->logger, "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", pcb_desalojado->pid);
}

void procesar_entradasalida_terminada(t_queue_block *interfaz, t_planificacion *kernel_argumentos)
{
    pthread_mutex_lock(&kernel_argumentos->colas.mutex_block);
    t_pcb *pcb_desalojado = queue_pop(interfaz->block_queue);
    kernel_argumentos->colas.cantidad_procesos_block--;
    pthread_mutex_unlock(&kernel_argumentos->colas.mutex_block);

    log_debug(kernel_argumentos->logger, "PID: %d", pcb_desalojado->pid);
    pcb_desalojado->estado = READY;

    if(pcb_desalojado->quantum != 0)
    {
        pthread_mutex_lock(&kernel_argumentos->colas.mutex_prioridad);
        queue_push(kernel_argumentos->colas.prioridad, pcb_desalojado);
        pthread_mutex_unlock(&kernel_argumentos->colas.mutex_prioridad);
    }
    else
    {
        pthread_mutex_lock(&kernel_argumentos->colas.mutex_ready);
        queue_push(kernel_argumentos->colas.ready, pcb_desalojado);
        pthread_mutex_unlock(&kernel_argumentos->colas.mutex_ready);
    }

    log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: BLOCK - Estado actual: READY", pcb_desalojado->pid);
    
    if(queue_is_empty(kernel_argumentos->colas.exec))
    {
        if(pthread_mutex_trylock(&kernel_argumentos->planning_mutex) == 0)
        {
            planificador_planificar(kernel_argumentos);
        }
    }
}


// -------- RECEPCION DE INTERFACES --------


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

void mover_a_exit(t_pcb* pcb_desalojado, t_planificacion *kernel_argumentos)
{
    t_proceso_estado aux = pcb_desalojado->estado;
    pcb_desalojado->estado = EXIT;

    pthread_mutex_lock(&kernel_argumentos->colas.mutex_exit);
    queue_push(kernel_argumentos->colas.exit, pcb_desalojado);
    pthread_mutex_unlock(&kernel_argumentos->colas.mutex_exit);

    char* estado = proceso_estado_a_string(aux);
    log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: %s - Estado actual: EXIT", pcb_desalojado->pid, estado);
}

char* proceso_estado_a_string(t_proceso_estado estado)
{
    if(estado == READY)
    {
        return "READY";
    }
    if(estado == NEW)
    {
        return "NEW";
    }
    if(estado == EXEC)
    {
        return "EXEC";
    }
    if(estado == BLOCKED)
    {
        return "BLOCK";
    }
    if(estado == EXIT)
    {
        return "EXIT";
    }
}