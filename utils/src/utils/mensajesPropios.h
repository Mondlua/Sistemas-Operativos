#ifndef UTILS_MENSAJESPROPIOS_H_
#define UTILS_MENSAJESPROPIOS_H_

#include<stdio.h>
#include<stdlib.h>
#include <utils/catedra/mensajes.h>

typedef enum
{
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ,
} IO_OPERATION;

typedef struct {
    uint32_t size; // Tamaño del payload
    uint32_t offset; // Desplazamiento dentro del payload
    void* stream; // Payload
} t_buffer_ins;

typedef struct
{
    IO_OPERATION codigo_operacion;
    t_buffer_ins* buffer;
} t_paquete_instruccion;

typedef struct 
{   
     union {
        struct {
            char* unidades_trabajo;
        } io_gen_sleep_params;
         struct {
            char* nombre_archivo;
        } io_fs_create_params;
     }params;
}instruccion_params;

typedef struct t_instruccion{

    op_code codigo_operacion;
    t_buffer_ins* buffer;
}t_instruccion;


void aviso_desconexion(char* , int);
void enviar_interfaz(char* mensaje, int socket_cliente);
char* recibir_interfaz(int socket_cliente, t_log* logger);
char* recibir_desconexion(int socket_cliente, t_log* logger);

//Ver nombres para no confundir
void enviar_instruccion(t_paquete_instruccion* , instruccion_params* , int );
instruccion_params* recibir_instruccion(int socket_servidor);
t_buffer_ins* serializar_io_gen_sleep(instruccion_params* param);
instruccion_params* deserializar_io_gen_sleep(t_buffer_ins* buffer);

t_instruccion* recibir_instruccion_cpu(int socket_servidor);
t_buffer_ins* serializar_instruccion(t_instruccion* ins);
void enviar_instruccion_mem(int socket_cliente, t_instruccion* instruccion);


void enviar_pc(char* pc, int socket_cliente);
char* recibir_pc(int socket_cliente);
char* recibo_pc(int socket_servidor);

#endif