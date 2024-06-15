#include "funciones_tlb.h"

t_list* lista_tlb; 

void iniciar_tlb(TLB *tlb) {
    tlb->entradas = (TLBEntry*)malloc(cant_entradas_tlb * sizeof(TLBEntry));
    for (int i = 0; i < cant_entradas_tlb; i++) {
        tlb->entradas[i].validez = false;  
        tlb->entradas[i].ultimo_acceso = 0;
    }
    tlb->siguiente_fifo = 0;
    tlb->contador_lru = 0;
}

int buscar_tlb(TLB* tlb, uint32_t pid, int num_pag){
  for (int i = 0; i < cant_entradas_tlb; i++) {
        if (tlb->entradas[i].validez && tlb->pid == pid && tlb->entradas[i].pagina == num_pag) {
            tlb->entradas[i].ultimo_acceso = tlb->time_counter++; 
            return tlb->entradas[i].frame; //HIT
        }
    }
    return -1;  //MISS
}

t_dir_fisica* posible_mmu(int dir_logica, uint32_t pid){

   //t_list* tabla_pags; // HACER TLB NECESARIO
    int numero_pagina = floor(dir_logica / tam_pag);
    int desplazamiento = dir_logica - numero_pagina * tam_pag;
    TLB* tlb = (TLB*)list_get(lista_tlb, pid); // calculo que como se van creando procesos y se añaden a la lista, el pid es igual a la posicion, REVEER
    int frame = buscar_tlb(tlb, pid, numero_pagina);
   if (frame != -1) {  // HIT
        printf("TLB Hit! Página: %d, Frame: %d\n", numero_pagina, frame);
    } else {  // MISS
        printf("TLB Miss!\n");
        // obtener la tabla de páginas correspondiente al pid
        // Obtener el frame desde la tabla de páginas
        // Actualizar TLB seg{un algoritmo}
        if (algoritmo == "FIFO") {
           // fifo(tlb, pid, numero_pagina, frame);
        } else if (algoritmo == "LRU") {
           // lru(tlb, pid, numero_pagina, frame);
        }
    }
   t_dir_fisica* direccionFisica = malloc(sizeof(t_dir_fisica));
   direccionFisica->nro_frame = frame;
   direccionFisica->desplazamiento = desplazamiento;
   return direccionFisica;
}
