#ifndef UTILSINICIO_H
#define UTILSINICIO_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <commons/log.h>
#include <commons/config.h>


typedef enum t_proceso_estado
{
    NEW,
    READY,
    EXEC,
    BLOCKED,
    EXIT
} t_proceso_estado;

typedef struct 
{    
    uint32_t PC; 
    uint8_t AX;   
    uint8_t BX;
    uint8_t CX;   
    uint8_t DX;   
    uint32_t EAX; 
    uint32_t EBX; 
    uint32_t ECX; 
    uint32_t EDX; 
    uint32_t SI;  
    uint32_t DI;  
} cpu_registros;

typedef struct t_pcb
{
    uint32_t pid;
    int quantum;
    cpu_registros* registros;
    t_proceso_estado estado; 

} t_pcb;


typedef struct t_dir_fisica{
    int nro_frame;
    int desplazamiento;
}t_dir_fisica;
typedef struct t_tabla{
    uint32_t pid;
    t_list* tabla;
}t_tabla;

t_log* iniciar_logger(char* ruta, char* emisor);
t_config* iniciar_config(char* ruta);
void inicializar_registro(t_pcb* pcb);

#endif