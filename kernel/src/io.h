#ifndef IO_H
#define IO_H

#include<signal.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/string.h>
#include<commons/log.h>
#include "server_kernel.h"


void validar_peticion(instruccion_params* parametros, t_pcb* pcb, int codigo_op, t_planificacion* kernel_argumentos);
void enviar_instruccion_a_interfaz(interfaz* interfaz_destino, instruccion_params* parametros, int cod_op);
interfaz* buscar_interfaz_por_nombre(char* nombre_interfaz);

t_instruccion_params_opcode recibir_solicitud_cpu(int socket_servidor, t_pcb* pcb);
instruccion_params* deserializar_io_gen_sleep_con_interfaz(t_buffer_ins* buffer);

#endif