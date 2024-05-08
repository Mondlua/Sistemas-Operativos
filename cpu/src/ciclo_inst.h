#ifndef CICLOINST_H_
#define CICLOINST_H_
#include "main.h"
#include <utils/mensajesPropios.h>

void inicializar_registro(t_pcb* pcb);
t_instruccion* fetch(t_pcb* pcb, int conexion_memoria);

#endif