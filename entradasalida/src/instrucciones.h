#ifndef INSTRUCIONEES_H_
#define INSTRUCCIONES_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <utils/catedra/mensajes.h>
#include "DialFS.h"

extern char* nombre_interfaz;
extern int conexion_kernel;
extern int conexion_memoria;


void recibir_instruccion(char*interfaz);
instruccion_params* deserializar_io_gen_sleep(t_buffer_ins* buffer);
instruccion_params* deserializar_registro_direccion_tamanio(t_buffer_ins* buffer);
instruccion_params* deserializar_io_fs_create_delete(t_buffer_ins* buffer);
instruccion_params* deserializar_io_fs_truncate(t_buffer_ins* buffer);
instruccion_params* deserializar_io_fs_write_read(t_buffer_ins* buffer);

int validar_operacion(char* tipo_interfaz, int codigo_operacion);
const char* op_code_a_string(instrucciones op_code);
void atender_cod_op(instruccion_params* parametros, instrucciones op_code, uint32_t pid);


#endif