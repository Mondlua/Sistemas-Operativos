#include "alg_planificacion.h"

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
            pcb_a_planificar = queue_pop(colaReady);
            pcb_a_planificar->estado=EXEC;
            queue_push(colaExec,pcb_a_planificar);
            enviar_pcb_cpu(pcb_a_planificar,conexion_cpu_dispatch);

            op_code motivo_desalojo = recibir_operacion(conexion_cpu_dispatch);
            pcb_actualizado = recibir_pcb(conexion_cpu_dispatch);

            pcb_a_planificar = queue_pop(colaExec);

            switch (motivo_desalojo) {
                case INS_EXIT:
                    cambiar_a_exit(pcb_actualizado);
                    break;
                case BLOCK_IO:
                    instruccion = recibir_instruccion_memoria(conexion_memoria);
                    instruccion_string = instruccion->buffer->stream;
                    array_palabras = string_split(instruccion_string, " ");

                    char* instruccionIO = array_palabras[0];
                    char* interfazIO = array_palabras[1];

                if(!validar_peticion(interfazIO)){ // Si no existe la interfaz o no esta conectada , el proceso pasa a EXIT
                    cambiar_a_exit(pcb_actualizado);
                    break;
                } else if(!validar_operacion(tipoInterfaz,instruccion)){ // Si no puede realizar la operacion solicitada, el proceso pasa a EXIT
                    cambiar_a_exit(pcb_actualizado);
                    break;
                } else if(interfaz_esta_libre()){
                    pcb_actualizado->estado=BLOCKED;
                    queue_push(colaBlocked, pcb_actualizado);
                    manejar_operacion_io(instruccion); // Un hilo para manejar las operaciones IO
                    break;
                }
                    break;
                case BLOCK_RECURSO:
                    instruccion = recibir_instruccion_memoria(conexion_memoria);
                    instruccion_string = instruccion->buffer->stream;
                    array_palabras = string_split(instruccion_string, " ");

                    char* instruccionRecurso = array_palabras[0];
                    char* recurso = array_palabras[1];

                if(!validar_recurso(recurso)){
                    cambiar_a_exit(pcb_actualizado);
                }
                    if(strcmp(instruccionRecurso, "WAIT")){
                        instanciasDisponibles = list_get(listaInstanciasRecursos, indiceRecurso);
                        instanciasDisponibles--;
                        if(instanciasDisponibles<0){
                            pcb_actualizado->estado=BLOCKED;
                            queue_push(colaBlocked, pcb_actualizado);
                            break;
                        }else{break;} 
                    }else if(strcmp(instruccionRecurso, "SIGNAL")){
                        instanciasDisponibles = list_get(listaInstanciasRecursos, indiceRecurso);
                        instanciasDisponibles++;
                        if(instanciasDisponibles>0){
                            //sacar de la cola de blocked del recurso
                            break;    
                    }else{break;} 
                    break;
                }
                
                
                
                default:
            }
            free(pcb_a_planificar);
            int tamanioReady = queue_size(colaReady);
            int tamanioExec = queue_size(colaExec);
        }
   }
//}

   //---------------------------------
  t_instruccion* recibir_instruccion_memoria(int socket_conexion){
    enviar_pc(int_to_char(pcb_a_planificar->p_counter),conexion_memoria);
    t_instruccion* instruccion = recibir_instruccion_cpu(conexion_memoria); // esta funcion tednria que tener nombre de recibir_instruccion_memoria
   }

   void manejar_operacion_io(t_instruccion* instruccion, char* interfazIO){
    enviar_instruccion();
    //enviar instruccion a la IO correspondiente
    //recibir notificacion de la IO
   }

void cambiar_a_ready(t_pcb* pcb){
    pcb->estado=READY;
    queue_push(colaReady, pcb);
}

void cambiar_a_exit(t_pcb* pcb){
    pcb->estado=EXIT;
    queue_push(colaExit, pcb);
}

void rr(int conexion_cpu_dispatch){

}