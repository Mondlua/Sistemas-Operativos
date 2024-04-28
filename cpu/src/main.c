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
   server_escuchar(cpu_log,"CPU",cpu_dispatch_server);

    // Inicio CPU INTERRUPT server

    cpu_interrupt_server = iniciar_servidor(puerto_cpu_interrupt, cpu_log);
    log_info(cpu_log, "CPU INTERRUPT listo para recibir a KERNEL");
    esperar_cliente(cpu_interrupt_server, cpu_log);

    return 0;

}


