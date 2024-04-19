#ifndef UTILSINICIO_H
#define UTILSINICIO_H

#include <stdlib.h>
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>

t_log* iniciar_logger(char* ruta, char* emisor);
t_config* iniciar_config(char* ruta);

#endif