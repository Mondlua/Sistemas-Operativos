#ifndef CICLOINST_H_
#define CICLOINST_H_
#include "main.h"
#include <utils/mensajesPropios.h>
#include <commons/collections/list.h>
#include <commons/string.h>


typedef enum instrucciones{
SET,
MOV_IN,
MOV_OUT, 
SUM,
SUB,
JNZ,
RESIZE,
COPY_STRING,
WAIT,
SIGNAL,
IO_GEN_SLEEP,
IO_STDIN_READ,
IO_STDOUT_WRITE,
IO_FS_CREATE,
IO_FS_DELETE,
IO_FS_TRUNCATE,
IO_FS_WRITE,
IO_FS_READ,
EXIIT
}instrucciones;


typedef struct t_decode {
instrucciones op_code;
t_list* registroCpu;
int valor;
instrucciones ins;
char* recurso;
bool logicaAFisica;
}t_decode;

t_instruccion* fetch(t_pcb* pcb, int conexion_memoria);
t_decode* decode(t_instruccion* ins);
instrucciones obtener_instruccion(char *nombre);
// Momentaneo

#endif