#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include "main.h"

void fifo(int conexion_cpu_dispatch);
void rr(int conexion_cpu_dispatch);
void cambiar_a_ready(t_pcb* pcb);
void cambiar_a_exit(t_pcb* pcb);

#endif