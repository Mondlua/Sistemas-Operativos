#ifndef MAINSERVERMEMORIA_H_
#define MAINSERVERMEMORIA_H_


#include "instrucciones_memoria.h"
#include <stdlib.h>
#include <stdio.h>
#include <utils/catedra/server.h>
#include <utils/catedra/client.h>
#include <inttypes.h>
#include <commons/log.h>
#include <pthread.h>
#include <utils/mensajesPropios.h>

extern t_list* lista_arch;

typedef struct
{
    t_log *log;
    int c_socket;
    char *server_name;

} t_atender_cliente_args;

int server_escuchar(void *arg);

#endif