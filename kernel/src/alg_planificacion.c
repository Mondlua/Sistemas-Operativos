#include "alg_planificacion.h"

typedef enum t_motivos_desalojo
{
    INS_EXIT,
    BLOCK_IO
    //BLOCK_RECURSO

} t_motivos_desalojo;

void fifo(int conexion_cpu_dispatch){
   int tamanioExec = queue_size(colaExec);
   int tamanioReady = queue_size(colaReady);
   int motivo_desalojo;
   t_pcb* pcb_actualizado;
   t_pcb* pcb_a_planificar;

    if(tamanioExec == 0 && tamanioReady > 0){
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
                    pcb_actualizado->estado=EXIT;
                    queue_push(colaExit, pcb_a_planificar);
                    break;
                case BLOCK_IO:
                pcb_actualizado->estado=BLOCKED;
                queue_push(colaBlocked, pcb_actualizado); // colaBlockedIO (*)
                
                if(!validar_conexion_IO(interfazIO)){ // Si no existe la interfaz o no esta conectada , el proceso pasa a EXIT
                    pcb_actualizado->estado=EXIT;
                    queue_push(colaExit, pcb_a_planificar);
                    break;
                } else if(validar_operacion()){ // Si no puede realizar la operacion solicitada, el proceso pasa a EXIT
                    pcb_actualizado->estado=EXIT;
                    queue_push(colaExit, pcb_a_planificar);
                    break;
                } else if(interfaz_esta_libre()){ // Creo que aca se podria usar "validar_peticion(interfaz, tiempo)" pero es void

                    //enviar instruccion a la IO correspondiente
                    //recibir notificacion de la IO
                    pcb_actualizado->estado=READY;
                    queue_push(colaReady, pcb_actualizado);
                    break;
                }

                }
                    break;
                /*case BLOCK_RECURSO:
                
                default:*/ // (*) Ver enunciado, ¿el estado BLOCKED tiene "multiples colas" una por cada IO y una por cada recurso?
            }
        }
   }