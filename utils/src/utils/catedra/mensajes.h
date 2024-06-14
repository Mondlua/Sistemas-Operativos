#ifndef UTILS_MENSAJES_H_
#define UTILS_MENSAJES_H_

//--------------FUNCIONES PROPIAS DE LA CATEDRA-------------------------//

#include<stdio.h>
#include<stdlib.h>
#include<commons/collections/list.h>
#include<commons/log.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <utils/catedra/inicio.h>

typedef enum
{
    MENSAJE,
    PAQUETE,
    INTERFAZ,
    CPU_RESIZE,
    PED_LECTURA,
    PED_ESCRITURA,
    CPY_STRING,
    FINALIZACION,
    TAM_PAG,
    AVISO_DESCONEXION,
    AVISO_OPERACION_INVALIDA,
    AVISO_OPERACION_VALIDADA,
    AVISO_OPERACION_FINALIZADA,
    PC,
    PID,
    INSTRUCCION,
    PCB,
    INS_EXIT,
    BLOCK_IO,
    BLOCK_RECURSO,
    FIN_QUANTUM,
    ACCESO_TABLA
} op_code;

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


void enviar_mensaje(char* , int );
void* recibir_buffer(int*, int);
char* recibir_mensaje(int, t_log*);
int recibir_operacion(int);

t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
t_list* recibir_paquete(int);

bool send_handshake(int conexion, t_log* logger, const char* conexion_name);
bool rcv_handshake(int fd_conexion);
void enviar_pedido_lectura(int socket_cliente,  t_dir_fisica* dir_fisica);

t_dir_fisica* recibir_pedido_lectura(int socket_cliente, t_log* logger);
void enviar_pedido_escritura(int socket_cliente,  t_dir_fisica* dir_fisica);
void enviar_valor_escritura(int socket_cliente,  uint8_t valor);
uint8_t recibir_valor_escritura(int socket_cliente, t_log* logger);
t_dir_fisica* recibir_pedido_escritura(int socket_cliente, t_log* logger);
void enviar_pedido_resize(int socket_cliente, int tampid);
int recibir_pedido_resize(int socket_cliente, t_log* logger);
void enviar_cpy_string(int socket_cliente, char* valor);
char* recibir_cpy_string(int socket_cliente, t_log* logger);
void enviar_pedido_tam_mem(int socket_cliente);
void enviar_tamanio_pag(int client_socket, int tam_pagina);
void recibir_tamanio_pag(int socket_cliente, t_log* logger, int* numero);
void recibir_ped_tamanio_pag(int socket_cliente, t_log* logger);
#endif