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

typedef enum
{
    MENSAJE,
    PAQUETE,
    INTERFAZ,
    CPU_MEMORIA_RESIZE,
    PED_LECTURA,
    PED_ESCRITURA,
    FINALIZACION,
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


#endif