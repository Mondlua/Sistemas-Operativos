#ifndef CICLOINST_H_
#define CICLOINST_H_
#include "main.h"
#include <utils/mensajesPropios.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <utils/io_operation.h>


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

typedef struct {
    int nro_pagina;
    int desplazamiento;
}t_dir_fisica;

t_instruccion* fetch(int conexion_memoria, t_pcb* pcb);
t_decode* decode(t_instruccion* ins);
void execute(t_decode* deacodeado, t_pcb* pcb);
instrucciones obtener_instruccion(char *nombre);
void realizar_ciclo_inst(int conexion_memoria, t_pcb* pcb);
t_dir_fisica* traducir_direc_logica(int tam_pag, uint32_t dir_logica);

#endif