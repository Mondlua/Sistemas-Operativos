#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <utils/client.h>
#include <utils/inicio.h>
#include <utils/comunicacion.h>
#include <consola.h>
#include<io.h>


t_log* kernel_log;
t_config* kernel_config;

t_queue* colaNew;
t_queue* colaReady;
t_queue* colaExec;
t_queue* colaBlocked;
t_queue* colaExit;

int nivel_multiprog=queue_size(colaReady)+queue_size(colaBlocked)+queue_size(colaExec);

int main(void)
{

    int kernel_server;
    char *puerto_escucha;

    char *ip_memoria;
    char *puerto_memoria;
    int conexion_memoria;

    char *ip_cpu;
    char *puerto_cpu_dispatch;
    char *puerto_cpu_interrupt;

    int conexion_cpu_dispatch;
    int conexion_cpu_interrupt;

    interfaces = list_create();

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


	ip_memoria = config_get_string_value(kernel_config,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(kernel_config, "PUERTO_MEMORIA");
	ip_cpu = config_get_string_value(kernel_config, "IP_CPU");
	puerto_cpu_dispatch = config_get_string_value(kernel_config, "PUERTO_CPU_DISPATCH");
	puerto_cpu_interrupt = config_get_string_value(kernel_config, "PUERTO_CPU_INTERRUPT");
    
    // Establecer conexiones

	/*conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);

    log_info(kernel_log, "KERNEL se conectó a MEMORIA");
    send_handshake(conexion_memoria, kernel_log, "KERNEL / MEMORIA");

    conexion_cpu_dispatch = crear_conexion(ip_cpu, puerto_cpu_dispatch);
    log_info(kernel_log, "KERNEL se conectó a CPU DISPATCH");
    send_handshake(conexion_cpu_dispatch, kernel_log, "KERNEL / CPU DISPATCH");

    // Pruebo enviar pcb a cpu
    inicializar_colas_estados();
    t_pcb* nuevo_pcb = iniciar_proceso();
    enviar_pcb_cpu(nuevo_pcb,conexion_cpu_dispatch);

    // Ver hilo
    conexion_cpu_interrupt = crear_conexion(ip_cpu, puerto_cpu_interrupt);
    log_info(kernel_log, "KERNEL se conectó a CPU INTERRUPT");
    send_handshake(conexion_cpu_interrupt, kernel_log, "KERNEL / CPU INTERRUPT");


    /* KERNEL - Servidor */



 	/* KERNEL - Servidor */

    // Extraer configs
    
   puerto_escucha = config_get_string_value(kernel_config, "PUERTO_ESCUCHA");

    // Inicio server
    
    kernel_server = iniciar_servidor(puerto_escucha, kernel_log);
    log_info(kernel_log, "KERNEL listo para recibir clientes");
    server_escuchar(kernel_log, "kernel", kernel_server);
    char *algoritmo=config_get_string_value(kernel_config, "ALGORITMO_PLANIFICACION");
    fifo();

    t_atender_cliente_args args;
    args.log = kernel_log;
    args.c_socket = kernel_server;
    args.server_name = "Kernel"; // Ejemplo de nombre de servidor
    pthread_t hilo;
    pthread_create(&hilo, NULL, (void *)server_escuchar, (void *)&args);
   
    consola_interactiva(kernel_log);
    //server_escuchar(kernel_log, "kernel", kernel_server);
    //pthread_t io = atender_peticion_IO("pepe", "34", kernel_server);
    sleep(10);
   // validar_peticion("pepe","33");
    pthread_join(hilo, NULL);

    return 0;
}