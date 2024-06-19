#ifndef TLB_H_
#define TLB_H
#include <utils/mensajesPropios.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <utils/Instrucciones_gral.h>
#include <math.h>
#include <stdbool.h>

typedef struct {
    uint32_t pid;
    int pagina;
    int frame;
    bool validez; //sería si está en uso o no esa entrada (ocupada)
    int ultimo_acceso;
} Entrada;


typedef struct {
    Entrada* entradas;
    int siguiente_fifo; 
    int contador_lru;
} TLB;

extern TLB* tlb;
extern int cant_entradas_tlb;
extern char* algoritmo; 
extern int tam_pag;

void iniciar_tlb();
int buscar_tlb(uint32_t pid, int num_pag);
t_dir_fisica* mmu(int dir_logica, uint32_t pid);
void remplazo_fifo(uint32_t pid, int pagina, int frame);
void remplazo_lru(uint32_t pid, int pagina, int frame);

#endif