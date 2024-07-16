#ifndef MAINCPU_H_
#define MAINCPU_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/catedra/client.h>
#include <utils/catedra/inicio.h>
#include "cpu_server.h"
#include <inttypes.h>
#include <pthread.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <utils/funcionesUtiles.h>
#include "funciones_tlb.h"

typedef struct{
    char *puerto_cpu_dispatch;
    char *puerto_cpu_interrupt;
    char *ip_cpu;
} t_config_leida_cpu;

typedef struct{
    t_config_leida_cpu config_leida;
    int socket_dispatch;
    int socket_interrupt;
} t_config_cpu;

void* conectar_dispatch(void* args);
void* conectar_interrupt(void* args);


#endif