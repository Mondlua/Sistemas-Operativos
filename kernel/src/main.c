#include <stdlib.h>
#include <stdio.h>
#include <utils/client.h>
#include <utils/inicio.h>
#include <utils/comunicacion.h>

int main(void) {
    int conexion_cpu_dispatch;
    int conexion_cpu_interrupt;
    int conexion_memoria;
    int kernel_server;
   // int quantum;
    // char* recursos[];
    // char* instancias_recursos[];
    //int grado_multiprogramacion;
    char* ip_memoria;
    char* ip_cpu;
    char* puerto_memoria;
    char* puerto_cpu_dispatch;
    char* puerto_cpu_interrupt;
	char* puerto_escucha;
    char* algoritmo_planificacion;
    t_log* kernel_log;
	t_config* kernel_config;


    kernel_log = iniciar_logger("kernel.log","kernel");
    kernel_config = iniciar_config("kernel.config");
    
	//Kernel como cliente
	ip_memoria = config_get_string_value(kernel_config,"IP_MEMORIA");
	ip_cpu = config_get_string_value(kernel_config, "IP_CPU");
	puerto_cpu_dispatch = config_get_string_value(kernel_config, "PUERTO_CPU_DISPATCH");
	puerto_cpu_interrupt = config_get_string_value(kernel_config, "PUERTO_CPU_INTERRUPT");
	puerto_memoria = config_get_string_value(kernel_config, "PUERTO_MEMORIA");
	puerto_escucha = config_get_string_value(kernel_config, "PUERTO_ESCUCHA");
    
    //conexiones
	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    log_info(kernel_log, "KERNEL se conectó a MEMORIA");
    send_handshake(conexion_memoria, kernel_log, "KERNEL / MEMORIA");
    
    conexion_cpu_dispatch = crear_conexion(ip_cpu, puerto_cpu_dispatch);
    log_info(kernel_log, "KERNEL se conectó a CPU dispatch");
    conexion_cpu_interrupt = crear_conexion(ip_cpu, puerto_cpu_interrupt);
    log_info(kernel_log, "KERNEL se conectó a CPU interrupt");
    send_handshake(conexion_cpu_dispatch, kernel_log, "KERNEL / CPU DISPATCH");
    send_handshake(conexion_cpu_interrupt, kernel_log, "KERNEL / CPU INTERRUPT");

    //kernel como servidor
    kernel_server = iniciar_servidor(puerto_escucha, kernel_log);
    log_info(kernel_log, "Modulo kernel listo para recibir clientes");
    server_escuchar(kernel_log, "Kernel", kernel_server);

    return 0;
}

