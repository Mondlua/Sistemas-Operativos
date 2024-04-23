#include <main.h>

int main(void) {

    t_log* kernel_log;
	t_config* kernel_config;    
    int kernel_server;
    char* puerto_escucha;

    char* ip_memoria;
    char* puerto_memoria;
    int conexion_memoria;

    char* ip_cpu;
    char* puerto_cpu_dispatch;
    char* puerto_cpu_interrupt;
	
    int conexion_cpu_dispatch;
    int conexion_cpu_interrupt;


    kernel_log = iniciar_logger("kernel.log","kernel");
    kernel_config = iniciar_config("kernel.config");
    

	/* KERNEL - Cliente */

    // Extraer configs

	ip_memoria = config_get_string_value(kernel_config,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(kernel_config, "PUERTO_MEMORIA");
    
	ip_cpu = config_get_string_value(kernel_config, "IP_CPU");
	puerto_cpu_dispatch = config_get_string_value(kernel_config, "PUERTO_CPU_DISPATCH");
	puerto_cpu_interrupt = config_get_string_value(kernel_config, "PUERTO_CPU_INTERRUPT");
    
    // Establecer conexiones

	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    log_info(kernel_log, "KERNEL se conectó a MEMORIA");
    send_handshake(conexion_memoria, kernel_log, "KERNEL / MEMORIA");
    
    conexion_cpu_dispatch = crear_conexion(ip_cpu, puerto_cpu_dispatch);
    log_info(kernel_log, "KERNEL se conectó a CPU DISPATCH");
    send_handshake(conexion_cpu_dispatch, kernel_log, "KERNEL / CPU DISPATCH");

    conexion_cpu_interrupt = crear_conexion(ip_cpu, puerto_cpu_interrupt);
    log_info(kernel_log, "KERNEL se conectó a CPU INTERRUPT");
    send_handshake(conexion_cpu_interrupt, kernel_log, "KERNEL / CPU INTERRUPT");


 	/* KERNEL - Servidor */

    // Extraer configs

    puerto_escucha = config_get_string_value(kernel_config, "PUERTO_ESCUCHA");

    // Inicio server

    kernel_server = iniciar_servidor(puerto_escucha, kernel_log);
    log_info(kernel_log, "KERNEL listo para recibir clientes");
    server_escuchar(kernel_log, "kernel", kernel_server);

    return 0;
}

// INICIAR PROCESO //

void iniciar_proceso (const char *nombre_archivo){

    FILE* archivo;
    t_pcb* pcb;

    archivo = fopen(nombre_archivo, "r");

    if (archivo == NULL) {
        printf("ERROR: No se pudo abrir el archivo.\n");
        return;
    }

    //CREAR PCB EN ESTADO NEW
    pcb = nuevo_pcb(1);
    
    //avisar a memoria

    fclose(archivo);
}

t_pcb* nuevo_pcb(int num_pid){

    t_pcb* nuevo_pcb = malloc(sizeof(t_pcb)); 
    
    if (nuevo_pcb == NULL) {
        return NULL;
      }
      
    nuevo_pcb->pid = num_pid;
    nuevo_pcb->p_counter = 0;
    nuevo_pcb->quantum = 0;/// EXTRAER DEL CONFIG 
    nuevo_pcb->tabla_paginas = NULL;  
    //nuevo_pcb->estado = NEW;   

    return nuevo_pcb;
}

void liberar_pcb(t_pcb* pcb) {
    if (pcb != NULL) {
        free(pcb->tabla_paginas);  
        free(pcb);            
    }
}