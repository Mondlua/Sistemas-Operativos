#include <stdlib.h>
#include <stdio.h>
#include <utils/client.h>
#include <utils/inicio.h>

int main(void) {
    int conexion_cpu_dispatch;
    int conexion_cpu_interrupt;
    int conexion_memoria;
    char* ip_memoria;
    char* ip_cpu;
    char* puerto_memoria;
    char* puerto_cpu_dispatch;
    char* puerto_cpu_interrupt;
	char* puerto_escucha;
    t_log* kernel_log;
	t_config* kernel_config;

    kernel_log = iniciar_logger("kernel.log","kernel");
    kernel_config = iniciar_config("kernel.config");
    log_info(kernel_log, "hola");
	
	ip_memoria = config_get_string_value(kernel_config,"IP_MEMORIA");
	ip_cpu = config_get_string_value(kernel_config, "IP_CPU");
	puerto_cpu_dispatch = config_get_string_value(kernel_config, "PUERTO_CPU_DISPATCH");
	puerto_cpu_interrupt = config_get_string_value(kernel_config, "PUERTO_CPU_INTERRUPT");
	puerto_memoria = config_get_string_value(kernel_config, "PUERTO_MEMORIA");
	puerto_escucha = config_get_string_value(kernel_config, "PUERTO_ESCUCHA");

	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    

    return 0;
}

