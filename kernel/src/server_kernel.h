#ifndef SERVER_H_
#define SERVER_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/server.h>
#include <utils/inicio.h>
#include <inttypes.h>
#include <commons/log.h>
#include <pthread.h>
#include <utils/mensajes.h>
#include<commons/collections/list.h>
#include <utils/Globals.h>

typedef struct
{
    t_log *log;
    int c_socket;
    char *server_name;

} t_atender_cliente_args;

typedef struct {
    char* nombre_interfaz;
    int socket_interfaz;  
} interfaz;

int server_escuchar(void *arg);
int buscar_interfaz_por_nombre(char* nombre_interfaz);
//t_pcb* recibir_pcb(int socket_cliente);


#endif