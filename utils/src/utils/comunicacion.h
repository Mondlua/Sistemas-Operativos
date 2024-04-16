#ifndef COMUNICACION_Mem_H_
#define COMUNICACION_Mem_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/server.h>
#include <utils/inicio.h>
#include <inttypes.h>
#include <commons/log.h>
#include <pthread.h>

int server_escuchar(t_log* logger, char* server_name, int server_socket);

#endif