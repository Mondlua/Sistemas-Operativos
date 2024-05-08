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
            
                break;
            case BLOCK_IO:
            if(tamanioReady < grado_multiprog){
                pcb_actualizado->estado=READY;
                queue_push(colaReady, pcb_actualizado);
            } else { 
                pcb_actualizado->estado=BLOCKED;
                queue_push(colaBlocked, pcb_actualizado); // colaBlockedIO (*)
             }
                break;
            /*case BLOCK_RECURSO:
            
            default:*/ // (*) Ver enunciado, ¿el estado BLOCKED tiene "multiples colas" una por cada IO y una por cada recurso?
        }
   }
   

}
