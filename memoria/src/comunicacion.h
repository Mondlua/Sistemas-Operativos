#ifndef COMUNICACION_Mem_H_
#define COMUNICACION_Mem_H_

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <commons/log.h>

int server_escuchar(t_log* logger, char* server_name, int server_socket);

#endif