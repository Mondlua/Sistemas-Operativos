#ifndef IO_H
#define IO_H

#include<signal.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/string.h>
#include<commons/log.h>
#include "server_kernel.h"


/*pthread_t atender_peticion_IO(char* interfaz, char* tiempo, int server);
void* validar_peticion(void*);*/
void validar_peticion(char* interfaz_a_validar, char* tiempo);

#endif