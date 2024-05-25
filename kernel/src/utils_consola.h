#ifndef UTILS_CONSOLAKERNEL_H_
#define UTILS_CONSOLAKERNEL_H_

#include <stdlib.h>
#include <stdio.h>
#include "server_kernel.h"
#include <utils/catedra/client.h>
#include <utils/catedra/inicio.h>
#include <utils/catedra/mensajes.h>
#include <inttypes.h>
#include <pthread.h>
#include <commons/collections/queue.h>
#include "main.h"
#include <utils/funcionesUtiles.h>

void inicializar_registro(t_pcb* pcb);
void inicializar_colas_estados();
t_pcb* crear_nuevo_pcb(uint32_t *pid_contador);
void mostrar_pids_en_estado(t_proceso_estado estado);
void enviar_pcb_cpu(t_pcb* pcb, int socket_cliente);
void liberar_pcb(t_pcb* pcb);
void inicializar_colas_estados();
t_queue* cola_pcb(uint32_t num_pid); //devuelve la cola en la que se encuentra el pcb
void borrar_pcb(uint32_t num_pid);
t_pcb* buscar_pcb(uint32_t num_pid);

#endif