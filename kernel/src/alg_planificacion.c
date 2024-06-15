#include "alg_planificacion.h"


sem_t *placeholder;

void* hilo_planificador(void *args)
{
    t_planificacion *kernel_argumentos = (t_planificacion*) args;

    t_tipo_planificacion algoritmo_planificador = obtener_algoritmo_planificador(kernel_argumentos->config.algoritmo_planificador);
    placeholder = &kernel_argumentos->planificar;
    while(1)
    {
        // Espero a que me soliciten planificar
        sem_wait(&kernel_argumentos->planificar);

        // Si se ejecuto DETENER_PLANIFICACION, no planifico por mas que me soliciten hacerlo
        if(kernel_argumentos->detener_planificacion)
        {
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

void iniciar_timer(t_timer_planificador timer, int milisegundos)
{
    timer_create(CLOCK_REALTIME, &timer.sev, &timer.timer);

    timer.its.it_value.tv_nsec = milisegundos * 1000;
    timer.its.it_value.tv_sec = 0;
    timer.its.it_interval.tv_nsec = 0;
    timer.its.it_interval.tv_sec = 0;

    timer_settime(timer.timer, 0, &timer.its, NULL);
}

int frenar_timer(t_timer_planificador timer)
{
    struct itimerspec remaining;
    timer_gettime(timer.timer, &remaining);
    timer_delete(timer.timer);
    return remaining.it_value.tv_nsec * 1000;
}

void planificador_planificar()
{
    sem_post(placeholder);
}

bool planificador_recepcion_pcb(t_pcb *pcb_desalojado, t_planificacion *kernel_argumentos)
{
    // Saco el pcb viejo de EXEC
    t_pcb *pcb_outdated = queue_pop(kernel_argumentos->colas.exec);

    if(pcb_desalojado->pid != pcb_outdated->pid)
    {   
        log_error(kernel_argumentos->logger, "Discordancia entre el pcb en EXEC y el ejecutado por CPU.");
        return;
    }

    // Meto el pcb a la cola correspondiente
    if(pcb_desalojado->estado == 0) // Desalojado por haber ejecutado EXIT
    {
        queue_push(kernel_argumentos->colas.exit, pcb_desalojado);
        // Enviar solicitud a memoria para desalojar el proceso
        log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: EXEC - Estado actual: READY", pcb_desalojado->pid);
    }
    if(pcb_desalojado->estado == 1) // Desalojado por fin de quantum
    {
        queue_push(kernel_argumentos->colas.ready, pcb_desalojado);
        log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: EXEC - Estado actual: READY", pcb_desalojado->pid);
    }
    if(pcb_desalojado->estado == 2) // Desalojado por IO_BLOCK
    {
        int milisegundos_restantes = frenar_timer(kernel_argumentos->timer_quantum);
        pcb_desalojado->quantum = milisegundos_restantes;
        // Mover a la cola de bloqueados correspondiente a la interfaz
        queue_push(kernel_argumentos, pcb_desalojado);
        log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: EXEC - Estado actual: BLOCK", pcb_desalojado->pid);
        // Realizar la solicitud correspondiente a IO
    }
    if(pcb_desalojado->estado == 3) // Desalojado por WAIT
    {
        // Verificar existencia y disponibilidad del recurso solicitado
        // Si no existe, a EXIT y return true.
        // Si existe, pero no hay instancias, a BLOCK correspondiente y return true
        // Si existe y hay instancias, se lo devuelve a EXEC, y se retorna un valor para que no se replanifique
        return false;
    }
    if(pcb_desalojado->estado == 4) // Desalojado por EXIT
    {
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
    cantidad_procesos_actual += queue_size(kernel_argumentos->colas.block);
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
    }
}

t_planificacion *inicializar_t_planificacion(int socket_cpu_dispatch, int socket_cpu_interrupt)
{
   t_planificacion *planificador = malloc(sizeof(t_planificacion));
    
    planificador->config.algoritmo_planificador = config_get_string_value(kernel_config, "ALGORITMO_PLANIFICACION");
    planificador->config.grado_multiprogramacion = config_get_int_value(kernel_config, "GRADO_MULTIPROGRAMACION");
    planificador->config.quantum = quantum;
    
    planificador->logger = kernel_log;

    planificador->colas.new = colaNew;
    planificador->colas.ready = colaReady;
    planificador->colas.exec = colaExec;
    planificador->colas.block = colaBlocked;
    planificador->colas.exit = colaExit;
    planificador->colas.prioridad = queue_create();

    planificador->detener_planificacion = 0;

    planificador->socket_cpu_dispatch = socket_cpu_dispatch;
    planificador->socket_cpu_interrupt = socket_cpu_interrupt;

    sem_init(&planificador->planificar, 0, 0);
    
    return planificador;
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
}

// ------- TRANSICIONES DE ESTADOS --------
t_pcb *planificador_ready_a_exec(t_planificacion *kernel_argumentos)
{
    
    t_pcb *proximo_proceso = queue_pop(kernel_argumentos->colas.ready);
    queue_push(kernel_argumentos->colas.exec, proximo_proceso);
    log_info(kernel_argumentos->logger, "PID: %d - Estado anterior: READY - Estado actual: EXEC", proximo_proceso->pid);
    return proximo_proceso;
}

t_pcb *planificador_prioridad_a_exec(t_planificacion *kernel_argumentos)
{
    t_pcb *proximo_pcb = queue_pop(kernel_argumentos->colas.prioridad);
    queue_push(kernel_argumentos->colas.exec, proximo_pcb);
    log_info(kernel_argumentos->logger, "PID %d - Estado anterior: READY - Estado actual: EXEC", proximo_pcb->pid);
    return proximo_pcb;
}
















/*int buscar_posicion_recurso_por_nombre(char* recurso){
    int tamanio_lista = list_size(recursos);
    for (int i = 0; i < tamanio_lista; i++) {
        char* posible_recurso = list_get(recursos, i);
        if (string_equals_ignore_case(posible_recurso, recurso)) {
            return i;
        }
    }
    return -1;
}
*/ //Falta hacer el manejo de recursos


// void fifo(int conexion_cpu_dispatch){
//    int tamanioReady = queue_size(colaReady);
//    int tamanioExec = queue_size(colaExec);
//    int motivo_desalojo;
//    t_pcb* pcb_actualizado;
//    t_pcb* pcb_a_planificar;
//    t_instruccion* instruccion;
//    char* instruccion_string;
//    char** array_palabras;

//     if(tamanioReady >= 0){
//        while(tamanioExec == 0 ){ //Cuando se implimente el comando "DETENER_PLANIFICACION", validar tambien que la planif no haya sido pausada
//             sem_wait(&cola_ready);
//             pcb_a_planificar = queue_pop(colaReady);
//             cambiar_a_cola(pcb_a_planificar,EXEC);
//             enviar_pcb(pcb_a_planificar,conexion_cpu_dispatch);
//              //recibir_interrupcion(conexion_cpu_interrupt); // VER
//             motivo_desalojo= recibir_operacion(conexion_cpu_dispatch);
//             motivo_desalojo = INS_EXIT;
//             pcb_a_planificar = recibir_pcb(conexion_cpu_dispatch);
//             log_info(kernel_log, "Recibi un PCB con AX %d y PC %d ", pcb_a_planificar->registros->AX,pcb_a_planificar->registros->PC) ;
//             //queue_pop(colaExec);
//             switch (motivo_desalojo) {
//                 case INS_EXIT:
//                     cambiar_a_cola(pcb_a_planificar, EXIT);
//                     borrar_pcb(pcb_a_planificar->pid);
//                     sem_post(&grado_planificiacion);
//                     //Falta que libere memoria y recursos
//                     break;
//                 case BLOCK_IO:
//                     instruccion_params* instruccion_io = malloc(sizeof(instruccion_params));
//                     recibir_solicitud_cpu(conexion_cpu_dispatch, pcb_actualizado);
//                 break;
                    
//                 case BLOCK_RECURSO: 
//                    /* instruccion = recibir_instruccion_memoria(conexion_memoria);
//                     instruccion_string = instruccion->buffer->stream;
//                     array_palabras = string_split(instruccion_string, " ");

//                     char* instruccionRecurso = array_palabras[0];
//                     char* recurso = array_palabras[1];

//                 if(!validar_recurso(recurso)){
//                     cambiar_a_cola(pcb_actualizado, EXIT);
//                 }
//                     if(strcmp(instruccionRecurso, "WAIT")){
//                         int indiceRecurso = buscar_posicion_recurso_por_nombre(recurso);
//                         int instanciasDisponibles = list_get(instanciasRecursos, indiceRecurso);
//                         instanciasDisponibles--;
//                         if(instanciasDisponibles<0){
//                             cambiar_a_cola(pcb->pcb_actualizado, BLOCKED);
//                             break;
//                         }else{break;} 
//                     }else if(strcmp(instruccionRecurso, "SIGNAL")){
//                         int indiceRecurso = buscar_posicion_recurso_por_nombre(recurso);
//                         int instanciasDisponibles = list_get(instanciasRecursos, indiceRecurso);
//                         instanciasDisponibles++;
//                         if(instanciasDisponibles>0){
//                             //sacar de la cola de blocked del recurso
//                             break;    
//                     }else{break;} 
//                     break;
//                 }
                
//                    }*/
                
//                 default:
//                 break;
                
//             }
            
//             tamanioReady =0; //queue_size(colaReady);
//             tamanioExec =0;//queue_size(colaExec);
//         }
//    }
//    }
    
//    //--------------------------------

// /*void rr(int conexion_cpu_dispatch){

//    int tamanioReady = queue_size(colaReady);
//    int tamanioExec = queue_size(colaExec);
//    op_code motivo_desalojo;
//    t_pcb* pcb_actualizado;
//    t_pcb* pcb_a_planificar;

//     if(tamanioReady > 0){
//        while( tamanioReady>0 && tamanioExec == 0 ){
        
//             pcb_a_planificar = queue_pop(colaReady);
//             cambiar_a_cola(pcb_a_planificar,EXEC);
//             enviar_pcb(pcb_a_planificar,conexion_cpu_dispatch);
//             pthread_t hiloquantum= pthread_create(&hiloquantum, NULL, manejar_quantum, pcb_a_planificar->pid);

//             //SEMAFOROS
//             motivo_desalojo = recibir_motivo(conexion_cpu_interrupt); 
//             pcb_actualizado = recibir_pcb(conexion_cpu_dispatch);

//             switch (motivo_desalojo) {
//                 case INS_EXIT://finalizo
//                 {
//                     borrar_pcb(pcb_actualizado->pid);  //Falta que libere memoria y recursos, 
//                     pthread_cancel(hiloquantum);
//                     pthread_destroy(hiloquantum);
//                     break;
//                 }
//                 case BLOCK_IO://block io
//                 {
//                     cambiar_a_cola(pcb_actualizado, BLOCKED);
//                     instruccion_params* instruccion_io = malloc(sizeof(instruccion_params));
//                     //instruccion_io = recibir_solicitud_cpu(conexion_cpu_dispatch);
//                     validar_peticion(instruccion_io->interfaz, instruccion_io->params.io_gen_sleep_params.unidades_trabajo, pcb_actualizado);
//                     break;
//                 }    
//                 case BLOCK_RECURSO://recursos VER
//                 { 
//                     cambiar_a_cola(pcb_actualizado, BLOCKED);
//                 }
//                 case FIN_QUANTUM://fin q
//                 {   
//                     log_info(kernel_log, "PID: <%u> - Desalojado por fin de Quantum", pcb_actualizado->pid);
//                     cambiar_a_cola(pcb_actualizado, READY);
//                     pthread_join(hiloquantum, NULL);
//                     pthread_destroy(hiloquantum);
//                 }
//                 default:{}     
//             }
        
//             tamanioReady = queue_size(colaReady);
//             tamanioExec = queue_size(colaExec);
//         }
//     }
// }


/*void* manejar_quantum(uint32_t pid){
    usleep(quantum);
    enviar_interrupcion_finq(pid,conexion_cpu_interrupt);
    
}

void enviar_interrupcion_finq(uint32_t pid, int conexion_cpu_interrupt){

    t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = FIN_QUANTUM;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(pid) + 1; // strlen a un uint? sizeof(uint32_t)
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, pid, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

    int resultado_send = send(conexion_cpu_interrupt, a_enviar, bytes, MSG_NOSIGNAL);

    if (resultado_send == -1) {
        fprintf(stderr, "Error al enviar el mensaje: socket cerrado.\n");
    }
    free(a_enviar);
    eliminar_paquete(paquete);
}

op_code recibir_motivo(int socket_cliente){
    int size;
    void* buffer;
    recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(size);
    recv(socket_cliente, buffer, size, MSG_WAITALL);
    return PCB;
}
*/