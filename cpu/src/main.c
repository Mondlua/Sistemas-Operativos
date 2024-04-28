/*#include<stdlib.h>
#include<stdio.h>
#include<utils/client.h>
#include<utils/inicio.h>
#include<utils/server.h> 
#include <utils/comunicacion.h>
*/

#include "main.h"

int main(void){

    t_log* cpu_log;
	t_config* cpu_config;   
    char* ip_cpu;
    
    int cpu_dispatch_server;
    int cpu_interrupt_server;
    char* puerto_cpu_dispatch;
    char* puerto_cpu_interrupt;

    char* ip_memoria;       
    char* puerto_memoria; 
    int conexion_memoria;


    cpu_log = iniciar_logger("cpu.log","cpu");
    cpu_config = iniciar_config("cpu.config");
    

	/* CPU - Cliente */

    // Extraer configs

	ip_memoria = config_get_string_value(cpu_config,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(cpu_config, "PUERTO_MEMORIA");
	
    // Establecer conexiones

	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    log_info(cpu_log, "CPU conectado a MEMORIA");
    send_handshake(conexion_memoria, cpu_log, "CPU / MEMORIA");
    

    /* CPU - Server */

    //Extraer configs

    puerto_cpu_dispatch = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_DISPATCH");
	puerto_cpu_interrupt = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_INTERRUPT");

    // Inicio CPU DISPATCH server  

    cpu_dispatch_server = iniciar_servidor(puerto_cpu_dispatch, cpu_log);
    log_info(cpu_log, "CPU DISPATCH listo para recibir a KERNEL");
   int conectado=esperar_cliente(cpu_dispatch_server, cpu_log);

    // Inicio CPU INTERRUPT server

    cpu_interrupt_server = iniciar_servidor(puerto_cpu_interrupt, cpu_log);
    log_info(cpu_log, "CPU INTERRUPT listo para recibir a KERNEL");
    esperar_cliente(cpu_interrupt_server, cpu_log);


    t_pcb* mandado=recibir_pcb(conectado);
    log_info(cpu_log,"recibi el pcb del proceso con pid %d",mandado->pid);
    return 0;

}


t_pcb* recibir_pcb(int socket_cliente) {
    t_paquete* paquete = recibir_paquete(socket_cliente);
    if (paquete == NULL) {
        return NULL;
    }

    t_pcb* pcb = malloc(sizeof(t_pcb));
    if (pcb == NULL) {
        return NULL;
    }

    int desplazamiento = 0;
    memcpy(&(pcb->pid), paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(pcb->p_counter), paquete->buffer->stream + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(&(pcb->quantum), paquete->buffer->stream + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(&(pcb->registros), paquete->buffer->stream + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(&(pcb->estado), paquete->buffer->stream + desplazamiento, sizeof(t_proceso_estado));
    desplazamiento += sizeof(t_proceso_estado);

    pcb->tabla_paginas = malloc(sizeof(int) * pcb->registros);
    memcpy(pcb->tabla_paginas, paquete->buffer->stream + desplazamiento, sizeof(int) * pcb->registros);
    desplazamiento += sizeof(int) * pcb->registros;

    pcb->algoritmo_planif = malloc(paquete->buffer->size - desplazamiento);
    memcpy(pcb->algoritmo_planif, paquete->buffer->stream + desplazamiento, paquete->buffer->size - desplazamiento);

    eliminar_paquete(paquete);

    return pcb;
}