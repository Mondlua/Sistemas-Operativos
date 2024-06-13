#include "alg_planificacion.h"


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


void fifo(int conexion_cpu_dispatch){
   int tamanioReady = queue_size(colaReady);
   int tamanioExec = queue_size(colaExec);
   int motivo_desalojo;
   t_pcb* pcb_actualizado;
   t_pcb* pcb_a_planificar;
   t_instruccion* instruccion;
   char* instruccion_string;
   char** array_palabras;

    if(tamanioReady >= 0){
       while(tamanioExec == 0 ){ //Cuando se implimente el comando "DETENER_PLANIFICACION", validar tambien que la planif no haya sido pausada
            sem_wait(&cola_ready);
            pcb_a_planificar = queue_pop(colaReady);
            cambiar_a_cola(pcb_a_planificar,EXEC);
            enviar_pcb(pcb_a_planificar,conexion_cpu_dispatch);
             //recibir_interrupcion(conexion_cpu_interrupt); // VER
            motivo_desalojo= recibir_operacion(conexion_cpu_dispatch);
            motivo_desalojo = INS_EXIT;
            pcb_a_planificar = recibir_pcb(conexion_cpu_dispatch);
            log_info(kernel_log, "Recibi un PCB con AX %d y PC %d ", pcb_a_planificar->registros->AX,pcb_a_planificar->registros->PC) ;
            //queue_pop(colaExec);
            switch (motivo_desalojo) {
                case INS_EXIT:
                    cambiar_a_cola(pcb_a_planificar, EXIT);
                    borrar_pcb(pcb_a_planificar->pid);
                    sem_post(&grado_planificiacion);
                    //Falta que libere memoria y recursos
                    break;
                case BLOCK_IO:
                    instruccion_params* instruccion_io = malloc(sizeof(instruccion_params));
                    instruccion_io = recibir_solicitud_cpu(conexion_cpu_dispatch);
                    validar_peticion(instruccion_io->interfaz, instruccion_io->params.io_gen_sleep_params.unidades_trabajo, pcb_actualizado);
                break;
                    
                case BLOCK_RECURSO: 
                   /* instruccion = recibir_instruccion_memoria(conexion_memoria);
                    instruccion_string = instruccion->buffer->stream;
                    array_palabras = string_split(instruccion_string, " ");

                    char* instruccionRecurso = array_palabras[0];
                    char* recurso = array_palabras[1];

                if(!validar_recurso(recurso)){
                    cambiar_a_cola(pcb_actualizado, EXIT);
                }
                    if(strcmp(instruccionRecurso, "WAIT")){
                        int indiceRecurso = buscar_posicion_recurso_por_nombre(recurso);
                        int instanciasDisponibles = list_get(instanciasRecursos, indiceRecurso);
                        instanciasDisponibles--;
                        if(instanciasDisponibles<0){
                            cambiar_a_cola(pcb->pcb_actualizado, BLOCKED);
                            break;
                        }else{break;} 
                    }else if(strcmp(instruccionRecurso, "SIGNAL")){
                        int indiceRecurso = buscar_posicion_recurso_por_nombre(recurso);
                        int instanciasDisponibles = list_get(instanciasRecursos, indiceRecurso);
                        instanciasDisponibles++;
                        if(instanciasDisponibles>0){
                            //sacar de la cola de blocked del recurso
                            break;    
                    }else{break;} 
                    break;
                }
                
                   }*/
                
                default:
                break;
                
            }
            
            tamanioReady =0; //queue_size(colaReady);
            tamanioExec =0;//queue_size(colaExec);
        }
   }

}

/*void rr(int conexion_cpu_dispatch){

   int tamanioReady = queue_size(colaReady);
   int tamanioExec = queue_size(colaExec);
   op_code motivo_desalojo;
   t_pcb* pcb_actualizado;
   t_pcb* pcb_a_planificar;

    if(tamanioReady > 0){
       while( tamanioReady>0 && tamanioExec == 0 ){
        
            pcb_a_planificar = queue_pop(colaReady);
            cambiar_a_cola(pcb_a_planificar,EXEC);
            enviar_pcb(pcb_a_planificar,conexion_cpu_dispatch);
            pthread_t hiloquantum= pthread_create(&hiloquantum, NULL, manejar_quantum, pcb_a_planificar->pid);

            //SEMAFOROS
            motivo_desalojo = recibir_motivo(conexion_cpu_interrupt); 
            pcb_actualizado = recibir_pcb(conexion_cpu_dispatch);

            switch (motivo_desalojo) {
                case INS_EXIT://finalizo
                {
                    borrar_pcb(pcb_actualizado->pid);  //Falta que libere memoria y recursos, 
                    pthread_cancel(hiloquantum);
                    pthread_destroy(hiloquantum);
                    break;
                }
                case BLOCK_IO://block io
                {
                    cambiar_a_cola(pcb_actualizado, BLOCKED);
                    instruccion_params* instruccion_io = malloc(sizeof(instruccion_params));
                    instruccion_io = recibir_solicitud_cpu(conexion_cpu_dispatch);
                    validar_peticion(instruccion_io->interfaz, instruccion_io->params.io_gen_sleep_params.unidades_trabajo, pcb_actualizado);
                    break;
                }    
                case BLOCK_RECURSO://recursos VER
                { 
                    cambiar_a_cola(pcb_actualizado, BLOCKED);
                }
                case FIN_QUANTUM://fin q
                {   
                    log_info(kernel_log, "PID: <%u> - Desalojado por fin de Quantum", pcb_actualizado->pid);
                    cambiar_a_cola(pcb_actualizado, READY);
                    pthread_join(hiloquantum);
                    pthread_destroy(hiloquantum);
                }
                default:{}     
            }
        
            tamanioReady = queue_size(colaReady);
            tamanioExec = queue_size(colaExec);
        }
}

void* manejar_quantum(uint32_t pid){
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
    buffer = malloc(*size);
    recv(socket_cliente, buffer, *size, MSG_WAITALL);
    log_info(logger, "Me llego el Motivo %d", buffer);
    return buffer;
}
*/