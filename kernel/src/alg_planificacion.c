#include "alg_planificacion.h"

void fifo(int conexion_cpu_dispatch){
   
   //cpu nos avisa que se libero
   t_pcb* pcb_aplanificar= queue_pop(colaReady);
    queue_push(colaExec,pcb_aplanificar);
    pcb_aplanificar->estado=EXEC;
    enviar_pcb_cpu(pcb_aplanificar,conexion_cpu_dispatch);
}