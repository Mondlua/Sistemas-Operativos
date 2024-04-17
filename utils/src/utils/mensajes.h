#ifndef UTILS_MENSAJES_H_
#define UTILS_MENSAJES_H_


#include<stdio.h>
#include<stdlib.h>
#include<commons/collections/list.h>
#include<commons/log.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>


typedef enum
{
    MENSAJE,
    PAQUETE,
    CREAR_PROCESO,
    FINALIZAR_PROCESO,
    ACCEDER_TABLAS_P,
    AJUSTAR_TAMANIO,
    ACCEDER_ESPACIO_U
}op_code;


typedef struct
{
    int size;
    void* stream;
} t_buffer;

typedef struct
{
    op_code codigo_operacion;
    t_buffer* buffer;
} t_paquete;




void* recibir_buffer(int*, int);
t_list* recibir_paquete(int);
void recibir_mensaje(int, t_log*);
int recibir_operacion(int);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);

#endif