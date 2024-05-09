#ifndef SERVER_H_
#define SERVER_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/catedra/server.h>
#include <utils/catedra/inicio.h>
#include <inttypes.h>
#include <commons/log.h>
#include <pthread.h>
#include <utils/mensajesPropios.h>
#include <commons/collections/list.h>
#include <semaphore.h>
#include <utils/io_operation.h>

extern t_list* interfaces;
extern sem_t sem_contador;

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