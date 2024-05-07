#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/catedra/server.h>
#include <utils/catedra/client.h>
#include <inttypes.h>
#include <commons/log.h>
#include <pthread.h>
#include <utils/Globals.h>
#include <utils/mensajesPropios.h>
#include </home/utnso/tp-2024-1c-Operati2/memoria/src/instrucciones_memoria.h>

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