#include <main.h>

t_log* kernel_log; 
t_config* kernel_config;
t_queue* colaNew;
t_queue* colaReady;
t_queue* colaExec;
t_queue* colaBlocked;
t_queue* colaExit;


int main(void) {

    //t_log* kernel_log;
	//t_config* kernel_config;    
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


    /* KERNEL - PCB */

    // Inicializo Colas de Estado //ver  si es necesario para execute y exit

    inicializar_colas_estados();
    iniciar_proceso();
    proceso_estado();

    
    return 0;
}

// INICIAR PROCESO //


void iniciar_proceso (/*const char *nombre_archivo*/){

    //FILE* archivo;
    t_pcb* pcb;

    // archivo = fopen(nombre_archivo, "r");

    /*if (archivo == NULL) {
        log_error(kernel_log, "No se pudo abrir el archivo.\n");
        return;
    }
*/
    static int pid_contador = 0;

    pcb = crear_nuevo_pcb(&pid_contador);
    queue_push(colaNew,pcb);

    //avisar a memoria

    //fclose(archivo);
}

t_pcb* crear_nuevo_pcb(int *pid_contador){

    t_pcb* nuevo_pcb = malloc(sizeof(t_pcb)); 
    
    if (nuevo_pcb == NULL) {
        log_warning(kernel_log,"Se creo un PCB NULL\n");
        return NULL;
    }
    
    nuevo_pcb->pid = *pid_contador;
    nuevo_pcb->p_counter = 0; //ver de donde salen todas las instrucciones
    nuevo_pcb->quantum =  config_get_int_value(kernel_config,"QUANTUM");
    nuevo_pcb->tabla_paginas = NULL;  
    nuevo_pcb->algoritmo_planif= config_get_string_value(kernel_config,"ALGORITMO_PLANIFICACION");
    nuevo_pcb->estado = NEW;
    
    log_info(kernel_log, "Se crea el proceso %d en NEW",*pid_contador);
    
    (*pid_contador)++;

    return nuevo_pcb;
}

void liberar_pcb(t_pcb* pcb) {
    if (pcb != NULL) {
        free(pcb->tabla_paginas);  
        free(pcb);            
    }
}

void finalizar_proceso(int pid){}

////CAMBIO MULTIPROGRAMACION /////

void multiprogramacion(int nuevo_grado){
    
    config_set_value(kernel_config,"GRADO_MULTIPROGRAMACION", (char*) nuevo_grado);

    int cambio_ok=config_get_int_value(kernel_config,"GRADO_MULTIPROGRAMACION");
    
    if(cambio_ok==nuevo_grado){
        log_info(kernel_log, "Se cambio el grado de multiprogramacion a %d", nuevo_grado);
    }else {
        log_warning(kernel_log, "No se pudo cambiar el grado de multiprogramacion");
    }
}

void inicializar_colas_estados(){
    colaNew = queue_create();
    colaReady = queue_create();
    colaExec = queue_create();
    colaBlocked = queue_create();
    colaExit = queue_create();
}

void proceso_estado(){
    
    // MOSTRAR COLA NEW
    // MOSTRAR COLA READY
    // MOSTRAR COLA EXEC
    // MOSTRAR COLA BLOCKED
    // MOSTRAS COLA EXIT

    int tamanio_cola = queue_size(colaNew);
    int contador = 0;
    
   // while(contador < tamanio_cola){
        t_pcb* pcb_apunta = colaNew->elements;
        int pid = pcb_apunta->pid;

        log_info(kernel_log, "PID: %d", pid);

        //contador++;
   // }

}

