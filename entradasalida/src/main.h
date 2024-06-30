#ifndef MAIN_IO_H
#define MAIN_IO_H


#include <stdlib.h>
#include <stdio.h>
#include <utils/catedra/client.h>
#include <utils/catedra/inicio.h>
#include <instrucciones.h>
#include "DialFS.h"

void extraer_segun_tipo_io(t_config* config, char* tipo_interfaz);
void conectar_con_memoria(t_config* config);

#endif
