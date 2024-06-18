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


t_instruccion* fetch(int conexion, t_pcb* pcb);
t_decode* decode(t_instruccion* ins);
void execute(t_decode* deacodeado, t_pcb* pcb);
instrucciones obtener_instruccion(char *nombre);
void realizar_ciclo_inst(int conexion, t_pcb* pcb);
//t_tabla* buscar_por_pid_return(uint32_t pid); 

#endif