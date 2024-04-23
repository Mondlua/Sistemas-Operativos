#ifndef MAINKERNEL_H_
#define MAINKERNEL_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/client.h>
#include <utils/inicio.h>
#include <utils/comunicacion.h>

#include <inttypes.h>
#include <pthread.h>

#include <commons/collections/queue.h>

extern t_log* kernel_log;
extern t_config* kernel_config;
typedef struct t_pcb
{
    uint32_t pid;
    int p_counter;
    int quantum;
    int registros[];// lu y martin
    proceso_estado estado; 
    int* tabla_paginas;
    char* algoritmo_planif;

} t_pcb;

typedef enum proceso_estado
{
    NEW,
    READY,
    EXEC,
    BLOCK,
    EXIT_PROCESS
} proceso_estado;



#endif