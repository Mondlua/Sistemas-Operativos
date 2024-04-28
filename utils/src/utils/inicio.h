#ifndef UTILSINICIO_H
#define UTILSINICIO_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <commons/log.h>
#include <commons/config.h>


typedef enum t_proceso_estado
{
    NEW,
    READY,
    EXEC,
    BLOCKED,
    EXIT
} t_proceso_estado;

typedef struct t_pcb
{
    uint32_t pid;
    int p_counter;
    int quantum;
    int registros;// lu y martin
    t_proceso_estado estado; 
    int* tabla_paginas;
    char* algoritmo_planif;

} t_pcb;

t_log* iniciar_logger(char* ruta, char* emisor);
t_config* iniciar_config(char* ruta);

#endif