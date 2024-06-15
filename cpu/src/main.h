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

extern int cant_entradas_tlb;
extern char algoritmo; 
extern int conexion_memoria_cpu;
extern t_log* cpu_log;
#endif