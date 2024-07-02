#ifndef IO_OP_H
#define IO_OP_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "Instrucciones_gral.h"
#include <utils/catedra/inicio.h>
#include "mensajesPropios.h"


typedef struct
{
    instrucciones codigo_operacion;
    t_buffer_ins* buffer;
} t_paquete_instruccion;

typedef struct 
{   
    char* interfaz;
    char* texto;
     union {
        struct {
            int unidades_trabajo;
        } io_gen_sleep_params;
        struct {
            t_dir_fisica* registro_direccion;
            uint32_t registro_tamaño;
        } io_stdin_stdout;
         struct {
            char* nombre_archivo;
        } io_fs_create_params;
        struct {
            char* nombre_archivo;
            uint32_t tamaño;
        } io_fs_truncate_params;
        struct {
            char* nombre_archivo;
            t_dir_fisica* registro_direccion;
            uint32_t registro_tamaño;
            void* registro_puntero_archivo;
        } io_fs_read_write;
     }params;
}instruccion_params;

t_buffer_ins* serializar_io_gen_sleep(instruccion_params* param);
t_buffer_ins* serializar_io_stdin_stdout(instruccion_params* param);
void enviar_instruccion(t_paquete_instruccion* instruccion, instruccion_params* parametros ,int socket_cliente, uint32_t pid);



//A KERNEL
t_buffer_ins* serializar_io_gen_sleep_con_interfaz(instruccion_params* param);
t_buffer_ins* serializar_io_stdin_stdout_con_interfaz(instruccion_params* param); 
void enviar_instruccion_a_Kernel(t_paquete_instruccion* instruccion, instruccion_params* parametros ,int socket_cliente);


//A MEMORIA
t_buffer_ins* serializar_io_stdin_con_texto(instruccion_params* param);
void enviar_instruccion_IO_Mem(t_paquete_instruccion* instruccion, instruccion_params* parametros, int socket_cliente);


#endif