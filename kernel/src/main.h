#ifndef MAINKERNEL_H_
#define MAINKERNEL_H_

#include "consola.h"
#include <stdlib.h>
#include <stdio.h>
#include "server_kernel.h"
#include <utils/catedra/client.h>
#include <utils/catedra/inicio.h>
#include "io.h"
#include <utils/funcionesUtiles.h>

extern t_log* kernel_log;
extern t_config* kernel_config;

extern  t_queue* colaNew;
extern  t_queue* colaReady;
extern  t_queue* colaExec;
extern  t_queue* colaBlocked;
extern  t_queue* colaExit;

extern int nivel_multiprog;

extern int conexion_memoria;
extern int conexion_cpu_dispatch;
extern int conexion_cpu_interrupt

extern int quantum;

#endif
