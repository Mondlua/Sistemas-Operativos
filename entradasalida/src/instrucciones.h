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

instruccion_params* recibir_instruccion(char*interfaz, int socket_servidor);
instruccion_params* deserializar_io_gen_sleep(t_buffer_ins* buffer);
int validar_operacion(char* tipo_interfaz, int codigo_operacion);


#endif