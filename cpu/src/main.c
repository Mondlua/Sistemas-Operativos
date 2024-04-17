#include<stdlib.h>
#include<stdio.h>
#include<utils/client.h>
#include<utils/inicio.h>
#include<utils/server.h>
#include <utils/comunicacion.h>

int main(void){
    int cpu_dispatch_server;
    int cpu_interrupt_server;
    int conexion_memoria;
    char* ip_memoria;
    char* ip_cpu;
    char* puerto_memoria;
    char* puerto_cpu_dispatch;
    char* puerto_cpu_interrupt;
  //  int cantidad_entradas_tlb;
  //  char* algortitmos_tlb;
    
    t_log* cpu_log;
	t_config* cpu_config;

    cpu_log = iniciar_logger("cpu.log","cpu");
    cpu_config = iniciar_config("cpu.config");
    
	// cliente a memoria
	ip_memoria = config_get_string_value(cpu_config,"IP_MEMORIA");
	puerto_cpu_dispatch = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_DISPATCH");
	puerto_cpu_interrupt = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_INTERRUPT");
	puerto_memoria = config_get_string_value(cpu_config, "PUERTO_MEMORIA");
	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    log_info(cpu_log, "CPU conectado a MEMORIA");
    send_handshake(conexion_memoria, cpu_log, "CPU / MEMORIA");
    
    // CPU DISPATCH server
    cpu_dispatch_server = iniciar_servidor(puerto_cpu_dispatch, cpu_log);
    log_info(cpu_log, "Modulo CPU dispatch lista para recibir a KERNEL");
    // server_escuchar(cpu_log, "cpu", cpu_dispatch_server);
    esperar_cliente(cpu_dispatch_server, cpu_log);

    // CPU INTERRUPT server

    //cpu_interrupt_server = iniciar_servidor(puerto_cpu_interrupt, cpu_log);
   // log_info(cpu_log, "Modulo CPU interrupt lista para recibir a KERNEL");
    //server_escuchar(cpu_log, "cpu",cpu_interrupt_server );

     
   // esperar_cliente(cpu_interrupt_server, cpu_log);

    return 0;

}
