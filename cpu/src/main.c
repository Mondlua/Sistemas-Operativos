#include "main.h"

int conexion_memoria_cpu;

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

    
    cpu_log = iniciar_logger("cpu.log","cpu");
    cpu_config = iniciar_config("cpu.config");
    

	/* CPU - Cliente */

    // Extraer configs

	ip_memoria = config_get_string_value(cpu_config,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(cpu_config, "PUERTO_MEMORIA");
	
    // Establecer conexiones
    
	conexion_memoria_cpu = crear_conexion(ip_memoria, puerto_memoria);
    log_info(cpu_log, "CPU conectado a MEMORIA");
    send_handshake(conexion_memoria_cpu, cpu_log, "CPU / MEMORIA");
    
    // Recibir PCB

    //Ejecutar Ciclo de Instruccion

    /* CPU - Server */
    
    //Extraer configs

    puerto_cpu_dispatch = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_DISPATCH");
	puerto_cpu_interrupt = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_INTERRUPT");

    // Inicio CPU DISPATCH server  

    cpu_dispatch_server = iniciar_servidor(puerto_cpu_dispatch, cpu_log);
    log_info(cpu_log, "CPU DISPATCH listo para recibir a KERNEL");
    
    t_atender_cliente_args* args = malloc(sizeof(t_atender_cliente_args));
    args->log = cpu_log;
    args->c_socket =cpu_dispatch_server;
    args->server_name = "CPU DISPATCH";
    server_escuchar(args);
    free(args);

    //preguntar a zoe si esto le ponemos el hilo extra como en memoria
    // Inicio CPU INTERRUPT server
    /*
    cpu_interrupt_server = iniciar_servidor(puerto_cpu_interrupt, cpu_log);
    log_info(cpu_log, "CPU INTERRUPT listo para recibir a KERNEL");
       
    t_atender_cliente_args* arg = malloc(sizeof(t_atender_cliente_args));
    arg->log = cpu_log;
    arg->c_socket =cpu_interrupt_server;
    arg->server_name = "CPU INTERRUPT";
    server_escuchar(arg);
    free(arg);
    */
//idem
    return 0;
}

