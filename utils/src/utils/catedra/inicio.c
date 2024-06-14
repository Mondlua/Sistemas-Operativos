#include "inicio.h"

t_log* iniciar_logger(char* ruta, char* emisor)
{
    t_log* nuevo_logger;

    nuevo_logger = log_create(ruta, emisor, true, LOG_LEVEL_DEBUG);
    if (nuevo_logger == NULL) {
        printf("No se pudo crear el logger");

        exit(1);
    }
    return nuevo_logger;
}

t_config* iniciar_config(char* ruta)
{
    t_config* nuevo_config;

    nuevo_config = config_create(ruta);
    if (nuevo_config == NULL) 
    {
        printf("No se pudo crear el config");

        exit(1);
    }
    
    return nuevo_config;
}


void inicializar_registro(t_pcb* pcb)
{

    pcb->registros= malloc(sizeof(cpu_registros));
    pcb->registros->PC = 0;
    pcb->registros->AX = 0;
    pcb->registros->BX = 0;
    pcb->registros->CX = 0;
    pcb->registros->DX = 0;
    pcb->registros->EAX = 0;
    pcb->registros->EBX = 0;
    pcb->registros->ECX = 0;
    pcb->registros->EDX = 0;
    pcb->registros->SI = 0;
    pcb->registros->DI = 0;
}
