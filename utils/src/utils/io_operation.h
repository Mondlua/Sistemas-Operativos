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
    t_dir_fisica* registro_direccion;
    uint32_t registro_tamanio;
     union {
        struct {
            int unidades_trabajo;
        } io_gen_sleep;
         struct {
            char* nombre_archivo;
            off_t registro_puntero_archivo;
        } io_fs;
     }params;
}instruccion_params;

t_buffer_ins* serializar_io_gen_sleep(instruccion_params* param);
t_buffer_ins* serializar_registro_direccion_tamanio(instruccion_params* param); //Tambien va con memoria
t_buffer_ins* serializar_io_fs_create_delete(instruccion_params* param);
t_buffer_ins* serializar_io_fs_truncate(instruccion_params* param);
t_buffer_ins* serializar_io_fs_write_read(instruccion_params* param);
void enviar_instruccion(t_paquete_instruccion* instruccion, instruccion_params* parametros ,int socket_cliente, uint32_t pid);



//A KERNEL
t_buffer_ins* serializar_io_gen_sleep_con_interfaz(instruccion_params* param);
t_buffer_ins* serializar_registro_direccion_tamanio_con_interfaz(instruccion_params* param); 
t_buffer_ins* serializar_io_fs_create_delete_con_interfaz(instruccion_params* param);
t_buffer_ins* serializar_io_fs_truncate_con_interfaz(instruccion_params* param);
t_buffer_ins* serializar_io_fs_write_read_con_interfaz(instruccion_params* param);
void enviar_instruccion_a_Kernel(t_paquete_instruccion* instruccion, instruccion_params* parametros ,int socket_cliente);


//A MEMORIA
t_buffer_ins* serializar_registro_direccion_tamanio_con_texto(instruccion_params* param);
void enviar_instruccion_IO_Mem(t_paquete_instruccion* instruccion, instruccion_params* parametros, int socket_cliente);


#endif