#include "alg_planificacion.h"

typedef enum t_motivos_desalojo // Los comparten el Kernel y el CPU
{
    INS_EXIT,
    BLOCK_IO,
    BLOCK_RECURSO

} t_motivos_desalojo;

void fifo(int conexion_cpu_dispatch){
   int tamanioReady = queue_size(colaReady);
   int motivo_desalojo;
   t_pcb* pcb_actualizado;
   t_pcb* pcb_a_planificar;
   t_instruccion* instruccion;
   char* instruccion_string;
   char** array_palabras;

    if(tamanioReady > 0){
        while( tamanioReady>0 ){ //Cuando se implimente el comando "DETENER_PLANIFICACION", validar tambien que la planif no haya sido pausada
            pcb_a_planificar = queue_pop(colaReady);
            pcb_a_planificar->estado=EXEC;
            queue_push(colaExec,pcb_a_planificar);
            enviar_pcb_cpu(pcb_a_planificar,conexion_cpu_dispatch);

            pcb_actualizado = recibir_pcb(conexion_cpu_dispatch);
            motivo_desalojo = recibir_motivo_desalojo(conexion_cpu_dispatch); // TO DO
            queue_pop(colaExec);
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

                if(!validar_conexion_IO(interfazIO)){ // Si no existe la interfaz o no esta conectada , el proceso pasa a EXIT
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

            tamanioReady = queue_size(colaReady);
        }
   }


   //---------------------------------
  t_instruccion* recibir_instruccion_memoria(int socket_conexion){
    enviar_pc(int_to_char(pcb_a_planificar->p_counter),conexion_memoria);
    t_instruccion* instruccion = recibir_instruccion_memoria(conexion_memoria); // esta funcion tiene el nombre de recibir_instruccion_cpu
   }

   void manejar_operacion_io(t_instruccion* instruccion, char* interfazIO){
    enviar_instruccion();
    //enviar instruccion a la IO correspondiente
    //recibir notificacion de la IO
   }

validar_conexion_IO(char* interfazIO){
    //asumo que vamos a hacer una lista de las interfaces que se van conectando al kernel
    

}


void cambiar_a_ready(t_pcb* pcb){
    pcb_actualizado->estado=READY;
    queue_push(colaReady, pcb);
}

void cambiar_a_exit(t_pcb* pcb){
    pcb_actualizado->estado=EXIT;
    queue_push(colaExit, pcb);
}