#ifndef UTILS_MENSAJESPROPIOS_H_
#define UTILS_MENSAJESPROPIOS_H_

#include<stdio.h>
#include<stdlib.h>
#include <utils/catedra/mensajes.h>
#include <utils/catedra/inicio.h>

typedef struct {
    uint32_t size; // Tamaño del payload
    uint32_t offset; // Desplazamiento dentro del payload
    void* stream; // Payload
} t_buffer_ins;


typedef struct t_instruccion{
    op_code codigo_operacion;
    t_buffer_ins* buffer;
}t_instruccion;


void aviso_desconexion(char* , int);
void enviar_interfaz(char* mensaje, int socket_cliente);
char* recibir_interfaz(int socket_cliente, t_log* logger);
char* recibir_desconexion(int socket_cliente, t_log* logger);
void aviso_operacion_invalida(char* mensaje, int socket);
void recibir_error_oi(int socket, t_log* logger);


t_instruccion* recibir_instruccion_cpu(int socket_servidor);
t_buffer_ins* serializar_instruccion(t_instruccion* ins);
void enviar_instruccion_mem(int socket_cliente, t_instruccion* instruccion);

t_pcb* recibir_pcb(int socket_cliente);
void enviar_pc(char* pc, int socket_cliente);
char* recibir_pc(int socket_cliente);

#endif