#ifndef CONSOLA_H
#define CONSOLA_H

#include<signal.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/string.h>
#include<commons/log.h>
#include<readline/readline.h>
#include "utils_consola.h"
#include "main.h"
//#include "alg_planificacion.h"

void consola_interactiva();
void*leer_consola();
void funciones(char* leido);
void ejecutar_script(char* path);
void iniciar_proceso(char* path);
void finalizar_proceso(uint32_t pid);
void iniciar_planificacion();
void detener_planificacion();
void multiprogramacion(char* valor);
void proceso_estado();



#endif
