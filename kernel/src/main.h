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

extern  t_queue* colaNew;
extern  t_queue* colaReady;
extern  t_queue* colaExec;
extern  t_queue* colaBlocked;
extern  t_queue* colaExit;

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

void iniciar_proceso (/*const char *nombre_archivo*/);

void proceso_estado();

void inicializar_colas_estados();

void multiprogramacion(int nuevo_grado);
t_pcb* crear_nuevo_pcb(uint32_t *pid_contador);
void mostrar_estado_cola(t_queue* cola, t_proceso_estado estado);
char* estado_a_string(t_proceso_estado estado);

#endif