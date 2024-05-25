#include "funcionesUtiles.h"

char* int_to_char(int num) {
    char *str;
    int len = snprintf(NULL, 0, "%d", num);
    str = (char *)malloc(len + 1); 
    snprintf(str, len + 1, "%d", num); 
    return str;
}

int find_queue(uint32_t elem, t_queue *cola)
{
    if (queue_is_empty(cola))
    {
        return 0;
    }

    t_link_element *actual = cola->elements->head;

    while (actual != NULL)
    {
        uint32_t *dato = (uint32_t*)actual->data;
        if (*dato == elem)
        {
            return 1;
        }
        actual = actual->next;
    }
    return 0;
}

void cambiar_a_cola(t_pcb* pcb, t_proceso_estado estado ){
    t_proceso_estado ant = pcb->estado;
    t_queue* colaAnt = cola_de_estado(pcb->estado);
    queue_pop(colaAnt);
    pcb->estado= estado;
    t_queue* colaNueva = cola_de_estado(estado);
    queue_push(colaReady, pcb);
    log_info(kernel_log, "PID: <%u> - Estado Anterior: <%d> - Estado Actual: <%d>", pcb->pid, estado_a_string(ant), estado_a_string(pcb->estado));
}

t_queue *cola_de_estado(t_proceso_estado estado)
{

    switch (estado)
    {
    case 0:
        return colaNew;
    case 1:
        return colaReady;
    case 2:
        return colaExec;
    case 3:
        return colaBlocked;
    case 4:
        return colaExit;
    default:
        return 0;
    }
}