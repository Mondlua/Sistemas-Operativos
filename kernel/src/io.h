#ifndef IO_H
#define IO_H

#include<signal.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/string.h>
#include<commons/log.h>
#include "server_kernel.h"

void validar_peticion(char* interfaz_a_validar, char* tiempo);
void enviar_instruccion_a_interfaz(interfaz* interfaz_destino, char* tiempo);
interfaz* buscar_interfaz_por_nombre(char* nombre_interfaz);

#endif