#include "alg_planificacion.h"


int buscar_posicion_recurso_por_nombre(char* recurso){
    int tamanio_lista = list_size(recursos);
    for (int i = 0; i < tamanio_lista; i++) {
        char* posible_recurso = list_get(recursos, i);
        if (string_equals_ignore_case(posible_recurso, recurso)) {
            return i;
        }
    }
    return -1;
}

void fifo(int conexion_cpu_dispatch){

   int tamanioReady = queue_size(colaReady);
   int tamanioExec = queue_size(colaExec);
   int motivo_desalojo;
   t_pcb* pcb_actualizado;
   t_pcb* pcb_a_planificar;
   t_instruccion* instruccion;
   char* instruccion_string;
   char** array_palabras;

    if(tamanioReady > 0){
       while( tamanioReady>0 && tamanioExec == 0 ){ //Cuando se implimente el comando "DETENER_PLANIFICACION", validar tambien que la planif no haya sido pausada
            cambiar_a_cola(pcb_a_planificar,EXEC);
            enviar_pcb_cpu(pcb_a_planificar,conexion_cpu_dispatch);

            motivo_desalojo = recibir_interrupcion(conexion_cpu_interrupt); VER
            pcb_actualizado = recibir_pcb(conexion_cpu_dispatch);

            switch (motivo_desalojo) {
                case INS_EXIT:
                    cambiar_a_cola(pcb_actualizado, EXIT);
                    borrar_pcb(pcb_actualizado->pid);
                    //Falta que libere memoria y recursos
                    break;
                case BLOCK_IO:
                    instruccion_params* instruccion_io = malloc(sizeof(instruccion_params));
                    recibir_solicitud_cpu(conexion_cpu_dispatch, pcb_actualizado);
                break;

                    
                case BLOCK_RECURSO:
                   { 
                    instruccion = recibir_instruccion_memoria(conexion_memoria);
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
                
                   }
                
                default:{ }
                
            }
            
            tamanioReady = queue_size(colaReady);
            tamanioExec = queue_size(colaExec);
        }
   }
    }
   //---------------------------------
  t_instruccion* recibir_instruccion_memoria(int socket_conexion){
    enviar_pc(int_to_char(pcb_actualizado->p_counter),conexion_memoria);
    t_instruccion* instruccion = recibir_instruccion_cpu(conexion_memoria); // esta funcion tednria que tener nombre de recibir_instruccion_memoria
   }

   void manejar_operacion_io(t_instruccion* instruccion, char* interfazIO){
    enviar_instruccion();
    //enviar instruccion a la IO correspondiente
    //recibir notificacion de la IO
   }

void rr(int conexion_cpu_dispatch){

   int tamanioReady = queue_size(colaReady);
   int tamanioExec = queue_size(colaExec);
   op_code motivo_desalojo;
   t_pcb* pcb_actualizado;
   t_pcb* pcb_a_planificar;

    if(tamanioReady > 0){
       while( tamanioReady>0 && tamanioExec == 0 ){
        
            pcb_a_planificar = queue_peek(colaReady);
            cambiar_a_cola(pcb_a_planificar,EXEC);
            pthread_t hiloquantum= pthread_create(&hiloquantum, NULL, manejar_quantum, pcb_a_planificar->pid);
            enviar_pcb_cpu(pcb_a_planificar,conexion_cpu_dispatch);

            //SEMAFOROS
            motivo_desalojo = recibir_motivo(conexion_cpu_interrupt); 
            pcb_actualizado = recibir_pcb(conexion_cpu_dispatch);

            switch (motivo_desalojo) {
                case 10://finalizo
                {
                    borrar_pcb(pcb->pid);  //Falta que libere memoria y recursos, 
                    pthread_cancel(hiloquantum);
                    pthread_destroy(hiloquantum);
                    break;
                }
                case 11://block io
                {
                    cambiar_a_cola(pcb_actualizado, BLOCKED);
                    instruccion_params* instruccion_io = malloc(sizeof(instruccion_params));
                    instruccion_io = recibir_solicitud_cpu(conexion_cpu_dispatch);
                    validar_peticion(instruccion_io->interfaz, instruccion_io->params.io_gen_sleep_params.unidades_trabajo);
                    break;
                }    
                case 12://recursos VER
                { 
                    cambiar_a_cola(pcb_actualizado, BLOCKED);
                }
                case 13://fin q
                {   
                    log_info(cpu_log, "PID: <%u> - Desalojado por fin de Quantum", pcb->pid);
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
}


void* manejar_quantum(uint32_t pid){
    usleep(quantum);
    enviar_interrupcion_finq(pid,conexion_cpu_interrupt);
    
}

void enviar_interrupcion_finq(uint32_t pid, int conexion_cpu_interrupt){

    t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = FIN_QUANTUM;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(pid) + 1;
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

op_code recibir_motivo(int conexion_cpu_interrupt){
    int size;
    void* buffer;
    recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
    recv(socket_cliente, buffer, *size, MSG_WAITALL);
    log_info(logger, "Me llego el Motivo %d", buffer);
    return buffer;
}
