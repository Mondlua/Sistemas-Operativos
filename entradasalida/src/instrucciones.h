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

extern char* nombre_interfaz;
extern int conexion_kernel;
extern int conexion_memoria;
extern int tiempo_unidad_trabajo;

void recibir_instruccion(char*interfaz);
instruccion_params* deserializar_io_gen_sleep(t_buffer_ins* buffer);
instruccion_params* deserializar_io_stdin_stdout(t_buffer_ins* buffer);
int validar_operacion(char* tipo_interfaz, int codigo_operacion);


#endif