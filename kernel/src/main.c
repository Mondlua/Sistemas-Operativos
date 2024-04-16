#include <stdlib.h>
#include <stdio.h>
#include <utils/client.h>
#include <utils/inicio.h>

int main(void) {
    int conexion_cpu_dispatch;
    int conexion_cpu_interrupt;
    int conexion_memoria;
    int quantum;
    // char* recursos[];
    // char* instancias_recursos[];
    int grado_multiprogramacion;
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
    
	//cliente a memoria
	ip_memoria = config_get_string_value(kernel_config,"IP_MEMORIA");
	ip_cpu = config_get_string_value(kernel_config, "IP_CPU");
	puerto_cpu_dispatch = config_get_string_value(kernel_config, "PUERTO_CPU_DISPATCH");
	puerto_cpu_interrupt = config_get_string_value(kernel_config, "PUERTO_CPU_INTERRUPT");
	puerto_memoria = config_get_string_value(kernel_config, "PUERTO_MEMORIA");
	puerto_escucha = config_get_string_value(kernel_config, "PUERTO_ESCUCHA");

	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    handshake_memoria(conexion_memoria,kernel_log);
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
//////// servidor kernel con i/o
  /*  char* puerto;
    int kernel_server;

    puerto = config_get_string_value(kernel_config ,"PUERTO_ESCUCHA");
    kernel_server = iniciar_servidor(puerto, kernel_log);
    log_info(kernel_log, "Modulo kernel lista para recibir a i/o");
    esperar_cliente(kernel_server, kernel_log);
*/
//cliente kernel con cpu
   /* ip_cpu = config_get_string_value(kernel_config,"IP_CPU");
	puerto_cpu_dispatch = config_get_string_value(kernel_config, "PUERTO_CPU_DISPATCH");
	puerto_cpu_interrupt = config_get_string_value(kernel_config, "PUERTO_CPU_INTERRUPT");
	puerto_escucha = config_get_string_value(kernel_config, "PUERTO_ESCUCHA");

    conexion_cpu_dispatch = crear_conexion(ip_cpu,puerto_cpu_dispatch);
 //   conexion_memoria = crear_conexion(ip_cpu,puerto_cpu_interrupt);

    return 0;
}

*/