#ifndef MAINKERNEL_H_
#define MAINKERNEL_H_

#include "consola.h"
#include <stdlib.h>
#include <stdio.h>
#include <utils/client.h>
#include <utils/inicio.h>
#include <utils/comunicacion.h>
#include <io.h>
extern t_log* kernel_log;
extern t_config* kernel_config;

extern  t_queue* colaNew;
extern  t_queue* colaReady;
extern  t_queue* colaExec;
extern  t_queue* colaBlocked;
extern  t_queue* colaExit;

extern int nivel_multiprog;

#endif