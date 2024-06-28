#ifndef INSTRUCIONEES_H_
#define INSTRUCCIONES_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include <utils/io_operation.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "DialFS.h"

extern char* nombre_interfaz;
extern t_log* entradasalida_log;
extern int conexion_kernel;
extern int conexion_memoria;


void recibir_instruccion(char*interfaz);
instruccion_params* deserializar_io_gen_sleep(t_buffer_ins* buffer);
instruccion_params* deserializar_io_stdin_stdout(t_buffer_ins* buffer);
int validar_operacion(char* tipo_interfaz, int codigo_operacion);
void atender_cod_op(instruccion_params* parametros, instrucciones op_code);


#endif