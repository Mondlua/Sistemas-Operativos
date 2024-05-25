#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include "main.h"

void fifo(int conexion_cpu_dispatch);
void rr(int conexion_cpu_dispatch);
void* manejar_quantum(uint32_t pid);
void enviar_interrupcion_finq(uint32_t pid, int conexion_cpu_interrupt);
op_code recibir_motivo( int conexion_cpu_interrupt);

#endif