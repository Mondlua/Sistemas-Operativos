#ifndef CONSOLA_H
#define CONSOLA_H

#include<signal.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/string.h>
#include<commons/log.h>
#include<readline/readline.h>
#include "utils_consola.h"
#include <utils/planificador.h>
//#include "main.h"
//#include "alg_planificacion.h"

void consola_interactiva(t_planificacion *kernel_argumentos);
void*leer_consola(void* args);
void funciones(char* leido, t_planificacion *kernel_argumentos);
void ejecutar_script(char* path, t_planificacion *kernel_argumentos);
void iniciar_proceso(char* path, t_planificacion *kernel_argumentos);
void finalizar_proceso(uint32_t pid, t_planificacion *kernel_argumentos);
void iniciar_planificacion(t_planificacion *kernel_argumentos);
void detener_planificacion(t_planificacion *kernel_argumentos);
void multiprogramacion(char* valor, t_planificacion *kernel_argumentos);
void proceso_estado(t_planificacion *kernel_argumentos);



#endif
