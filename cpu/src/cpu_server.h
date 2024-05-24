#ifndef CPUSERVER_H_
#define CPUSERVER_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/catedra/server.h>
#include <utils/catedra/client.h>
#include <inttypes.h>
#include <commons/log.h>
#include <pthread.h>
#include <utils/mensajesPropios.h>
#include <utils/funcionesUtiles.h>
#include "main.h"
#include "ciclo_inst.h"

extern int kernel_socket;
typedef struct
{
    t_log *log;
    int c_socket;
    char *server_name;

} t_atender_cliente_args;

int server_escuchar(void *arg);

#endif
