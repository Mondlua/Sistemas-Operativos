#ifndef CPUSERVER_H_
#define CPUSERVER_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/catedra/server.h>
#include <utils/catedra/client.h>
#include <inttypes.h>
#include <commons/log.h>
#include <pthread.h>
#include <utils/mensajesPropios.h>
#include <utils/funcionesUtiles.h>
#include "main.h"
#include "ciclo_inst.h"

extern int hay_interrupcion;
extern int kernel_socket;
typedef struct t_atender_cliente_args
{
    t_log *log;
    int c_socket;
    char *server_name;
    pthread_mutex_t lock_interrupt;
    
} t_atender_cliente_args;

void atender_cliente(void* void_args);
int server_escuchar(void *arg);
void enviar_motivo(op_code FIN_QUANTUM, int kernel_socket);
void recibir_interrupcion_finq(int socket_cliente);

#endif
