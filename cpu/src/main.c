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

    t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->pid = 0;
    pcb->p_counter=2;

    t_instruccion* ins = fetch(pcb, conexion_memoria);
    
    /* CPU - Server */
    /*
    //Extraer configs

    puerto_cpu_dispatch = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_DISPATCH");
	puerto_cpu_interrupt = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_INTERRUPT");

    // Inicio CPU DISPATCH server  

    cpu_dispatch_server = iniciar_servidor(puerto_cpu_dispatch, cpu_log);
    log_info(cpu_log, "CPU DISPATCH listo para recibir a KERNEL");
    
    t_atender_cliente_args* args = malloc(sizeof(t_atender_cliente_args));
    args->log = cpu_log;
    args->c_socket =cpu_dispatch_server;
    args->server_name = "CPU";
    server_escuchar(args);
    free(args);
    // Inicio CPU INTERRUPT server

    cpu_interrupt_server = iniciar_servidor(puerto_cpu_interrupt, cpu_log);
    log_info(cpu_log, "CPU INTERRUPT listo para recibir a KERNEL");
    esperar_cliente(cpu_interrupt_server, cpu_log);

    */
    return 0;

}

char* int_to_char(int num) {
    char *str;
    int len = snprintf(NULL, 0, "%d", num); // Obtener la longitud de la cadena necesaria
    str = (char *)malloc(len + 1); // Asignar memoria para la cadena de caracteres
    snprintf(str, len + 1, "%d", num); // Convertir el entero a una cadena de caracteres
    return str;
}
