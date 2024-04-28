#ifndef MAINKERNEL_H_
#define MAINKERNEL_H_

#include "f_consola.h"

extern t_log* kernel_log;
extern t_config* kernel_config;

extern  t_queue* colaNew;
extern  t_queue* colaReady;
extern  t_queue* colaExec;
extern  t_queue* colaBlocked;
extern  t_queue* colaExit;

#endif