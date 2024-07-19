#ifndef FUNCIONESUTILES_H_
#define FUNCIONESUTILES_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/collections/queue.h>
#include <utils/catedra/inicio.h>
#include <inttypes.h>
#include <pthread.h>

char* int_to_char(int num);
int find_queue(uint32_t elem, t_queue *cola);
char* estado_a_string(t_proceso_estado estado);
/*oid cambiar_a_cola(t_pcb* pcb, t_proceso_estado estado);
t_queue *cola_de_estado(t_proceso_estado estado);
*/
#endif