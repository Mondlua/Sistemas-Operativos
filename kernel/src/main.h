#ifndef MAINKERNEL_H_
#define MAINKERNEL_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/client.h>
#include <utils/inicio.h>
#include <utils/comunicacion.h>

#include <inttypes.h>
#include <commons/log.h>
#include <pthread.h>

typedef struct
{
    uint32_t pid;
    int p_counter;
    int quantum;
    int registers[];
   // char* estado; //zoe
    int* tabla_paginas;

} t_pcb;



#endif