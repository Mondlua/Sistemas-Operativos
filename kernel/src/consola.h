#ifndef CONSOLA_H
#define CONSOLA_H

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/string.h>
#include<commons/log.h>
#include<readline/readline.h>
#include<pthread.h>
#include "utils_consola.h"
#include "main.h"

void consola_interactiva(t_log* logger);
void* leer_consola(void *arg);
void funciones(char* leido, t_log* logger);
void ejecutar_script(char* path, t_log* logger);
void iniciar_proceso(char* path, t_log* logger);
void finalizar_proceso(uint32_t pid, t_log* logger);
void iniciar_planificacion(t_log* logger);
void detener_planificacion(t_log* logger);
void multiprogramacion(char* valor, t_log* logger);
void proceso_estado(t_log* logger);



#endif
