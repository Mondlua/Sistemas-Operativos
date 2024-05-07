#ifndef IO_H
#define IO_H

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/string.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<utils/mensajes.h>
#include<utils/comunicacion.h>
#include<utils/Globals.h>

typedef struct
{
    char* interfaz_solicitada;
    int timer;
    int cliente;

} atender_peticion_args;


/*pthread_t atender_peticion_IO(char* interfaz, char* tiempo, int server);
void* validar_peticion(void*);*/
void validar_peticion(char* interfaz_a_validar, char* tiempo);

#endif