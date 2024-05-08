#ifndef UTILS_MENSAJES_H_
#define UTILS_MENSAJES_H_


#include<stdio.h>
#include<stdlib.h>
#include<commons/collections/list.h>
#include<commons/log.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

typedef enum
{
    MENSAJE,
    PAQUETE,
    INTERFAZ,
    AVISO_DESCONEXION
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





void aviso_desconexion(char* , int);
void* recibir_buffer(int*, int);
t_list* recibir_paquete(int);
void recibir_mensaje(int, t_log*);
int recibir_operacion(int);
void enviar_mensaje(char* , int );
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void enviar_interfaz(char* mensaje, int socket_cliente);
char* recibir_interfaz(int socket_cliente, t_log* logger);
bool rcv_handshake(int fd_conexion);
bool send_handshake(int conexion, t_log* logger, const char* conexion_name);
char* recibir_desconexion(int socket_cliente, t_log* logger);


#endif