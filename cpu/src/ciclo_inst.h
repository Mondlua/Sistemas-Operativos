#ifndef CICLOINST_H_
#define CICLOINST_H_
#include <utils/mensajesPropios.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <utils/io_operation.h>
#include "main.h"
#include <math.h>
//#include "/home/utnso/tp-2024-1c-Operati2/memoria/src/main.h"

extern int kernel_socket;
typedef struct t_decode {
instrucciones op_code;
t_list* registroCpu;
int valor;
char* recurso;
char* interfaz;
char* archivo;
bool logicaAFisica;
instrucciones instrucciones;
}t_decode;
/*
typedef struct {
    const char* nombre;
    void* puntero;
    size_t tamano; 
} registro_mapa;*/

typedef enum {
    NO_BLOCK,
    IO_BLOCK,
    REC_BLOCK_WAIT,
    REC_BLOCK_SIGNAL,
    EXIT_BLOCK
} t_cpu_blockeo_enum;

typedef struct
{
    t_cpu_blockeo_enum blockeo;
    instrucciones io_opcode;
    instruccion_params* instrucciones;
    char* nombre_recurso;
} t_cpu_blockeo;


char* fetch(int conexion, t_pcb* pcb);

t_decode* decode(char* ins);
t_cpu_blockeo execute(t_decode* deacodeado, t_pcb* pcb, t_log *logger);
instrucciones obtener_instruccion(char *nombre);
void realizar_ciclo_inst(int conexion, t_pcb* pcb, t_log *logger, int socket_cliente, pthread_mutex_t lock_interrupt);
void loggear_registros(t_pcb* pcb, t_log* logger);
void* obtener_valor_registro(cpu_registros* regs, char* nombre_registro);


#endif
