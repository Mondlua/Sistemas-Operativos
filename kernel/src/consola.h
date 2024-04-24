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


void leer_consola(t_log* logger);
void funciones(char* leido, t_log* logger);
void ejecutar_script(char* path, t_log* logger);
void iniciar_proceso(char* path, t_log* logger);
void finalizar_proceso(int pid, t_log* logger);
void iniciar_planificacion(t_log* logger);
void detener_planificacion(t_log* logger);
void multiprogramacion(int valor, t_log* logger);
void proceso_estado(t_log* logger);


#endif
