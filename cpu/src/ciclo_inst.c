#include "ciclo_inst.h"

void inicializar_registro(t_pcb* pcb)
{
    cpu_registros *registros = pcb->registros;
    registros= malloc(sizeof(cpu_registros));
    registros->PC = malloc(sizeof(4));
    registros->AX = malloc(sizeof(1));
    registros->BX = malloc(sizeof(1));
    registros->CX = malloc(sizeof(1));
    registros->DX = malloc(sizeof(1));
    registros->EAX = malloc(sizeof(4));
    registros->EBX = malloc(sizeof(4));
    registros->ECX = malloc(sizeof(4));
    registros->EDX = malloc(sizeof(4));
    registros->SI = malloc(sizeof(4));
    registros->DI = malloc(sizeof(4));
}

/*t_instruccion* fetch(t_pcb* pcb, int conexion_memoria){
    
    t_instruccion* instruccion;

    enviar_pc(int_to_char(pcb->pc),conexion_memoria); //Mando PC
    instruccion = recibo_instruccion(conexion_memoria);//Recibo INSTRUCCION

    printf("Recibi la INSTRUCCION %s", instruccion->buffer->stream);
    
    pcb->p_counter++;
    return instruccion;
}*/