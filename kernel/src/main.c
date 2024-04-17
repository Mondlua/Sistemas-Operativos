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
    log_info(kernel_log, "Kernel se conectó a memoria");
    conexion_cpu_dispatch = crear_conexion(ip_cpu, puerto_cpu_dispatch);
    log_info(kernel_log, "Kernel se conectó a cpu dispatch");
    conexion_cpu_interrupt = crear_conexion(ip_cpu, puerto_cpu_interrupt);
    log_info(kernel_log, "Kernel se conectó a cpu interrupt");

    //kernel como servidor
    kernel_server = iniciar_servidor(puerto_escucha, kernel_log);
    log_info(kernel_log, "Modulo kernel listo para recibir clientes");
    server_escuchar(kernel_log, "Kernel", kernel_server);


   // handshake_memoria(conexion_memoria,kernel_log);
    return 0;
}

void handshake_memoria(int conexion_memoria, t_log* kernel_log){
    size_t bytes;

    int32_t handshake = 1;
    int32_t result;

    bytes = send(conexion_memoria, &handshake, sizeof(int32_t), 0);
    bytes = recv(conexion_memoria, &result, sizeof(int32_t), MSG_WAITALL);

    if (result == 0) {
        log_info(kernel_log, "hanshake ok");
    // Handshake OK
    } else {
    // Handshake ERROR
    log_info(kernel_log,"error handshake");
}
}