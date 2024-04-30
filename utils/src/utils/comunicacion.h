#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/server.h>
#include <utils/inicio.h>
#include <inttypes.h>
#include <commons/log.h>
#include <pthread.h>
#include <utils/mensajes.h>

typedef struct
{
    t_log *log;
    int c_socket;
    char *server_name;

} t_atender_cliente_args;

extern t_list* interfaces;
//extern int fd_interfaz;

int server_escuchar(void *arg);
bool rcv_handshake(int fd_conexion);
bool send_handshake(int conexion, t_log* logger, const char* conexion_name);
//t_pcb* recibir_pcb(int socket_cliente);


#endif