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

typedef struct 
{
    uint32_t PC;  // Program Counter
    uint8_t AX;   // Registro Numérico de propósito general
    uint8_t BX;   // Registro Numérico de propósito general
    uint8_t CX;   // Registro Numérico de propósito general
    uint8_t DX;   // Registro Numérico de propósito general
    uint32_t EAX; // Registro Numérico de propósito general
    uint32_t EBX; // Registro Numérico de propósito general
    uint32_t ECX; // Registro Numérico de propósito general
    uint32_t EDX; // Registro Numérico de propósito general
    uint32_t SI;  // Contiene la dirección lógica de memoria de origen para copiar un string
    uint32_t DI;  // Contiene la dirección lógica de memoria de destino para copiar un string
} cpu_registros;
typedef struct t_pcb
{
    uint32_t pid;
    int p_counter;
    int quantum;
    cpu_registros* registros;
    t_proceso_estado estado; 
    int* tabla_paginas;
    char* algoritmo_planif;

} t_pcb;

t_log* iniciar_logger(char* ruta, char* emisor);
t_config* iniciar_config(char* ruta);

#endif