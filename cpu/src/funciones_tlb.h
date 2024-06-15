#ifndef TLB_H_
#define TLB_H
#include <utils/mensajesPropios.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <utils/Instrucciones_gral.h>
#include "ciclo_inst.h"
#include "main.h"
#include <math.h>
#include <stdbool.h>


typedef struct {
    int pagina;
    int frame;
    bool validez; //sería si está en uso o no esa entrada (ocupada)
    int ultimo_acceso;
} TLBEntry;

typedef struct {
    uint32_t pid;
    TLBEntry* entradas;
    int siguiente_fifo; 
    int contador_lru;
} TLB;


void iniciar_tlb(TLB* tlb);
int buscar_tlb(TLB* tlb, uint32_t pid, int num_pag);
t_dir_fisica* posible_mmu(int dir_logica, uint32_t pid);

#endif