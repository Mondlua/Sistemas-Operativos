#ifndef CICLOINST_H_
#define CICLOINST_H_
#include <utils/mensajesPropios.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <utils/io_operation.h>
#include "main.h"
#include <math.h>

typedef struct t_decode {
instrucciones op_code;
t_list* registroCpu;
int valor;
char* recurso;
char* interfaz;
bool logicaAFisica;
instrucciones instrucciones;
}t_decode;

typedef struct {
    const char* nombre;
    void* puntero;
    size_t tamano; 
} registro_mapa;

typedef enum {
    NO_BLOCK,
    IO_BLOCK,
    REC_BLOCK,
    EXIT_BLOCK
} t_cpu_blockeo;

t_instruccion* fetch(int conexion, t_pcb* pcb);
t_decode* decode(t_instruccion* ins);
t_cpu_blockeo execute(t_decode* deacodeado, t_pcb* pcb, t_log *logger);
instrucciones obtener_instruccion(char *nombre);
void realizar_ciclo_inst(int conexion, t_pcb* pcb, t_log *logger, int socket_cliente, pthread_mutex_t lock_interrupt);
void loggear_registros(t_pcb* pcb, t_log* logger);
char** split_por_bytes(const char* string, size_t bytes, int* cant_partes);
//t_tabla* buscar_por_pid_return(uint32_t pid); 

#endif