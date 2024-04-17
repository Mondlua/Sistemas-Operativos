#include <stdlib.h>
#include <stdio.h>
#include <utils/client.h>
#include <utils/inicio.h>
#include <utils/comunicacion.h>

int main(void) {
    int conexion_memoria;
    int conexion_kernel;
    char* ip_kernel; 
    char* ip_memoria;
    char* puerto_memoria;
    char* puerto_kernel;
    

    t_log* entradasalida_log;
	t_config* entradasalida_config;

    entradasalida_log = iniciar_logger("entradasalida.log","entradasalida");
    entradasalida_config = iniciar_config("entradasalida.config");
    
	//Entrada salida como cliente
	ip_memoria = config_get_string_value(entradasalida_config,"IP_MEMORIA");
    ip_kernel= config_get_string_value(entradasalida_config,"IP_KERNEL");
    puerto_memoria = config_get_string_value(entradasalida_config, "PUERTO_MEMORIA");
    puerto_kernel = config_get_string_value(entradasalida_config, "PUERTO_KERNEL");

    //Conexiones
    conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    log_info(entradasalida_log, "I/O conectado a MEMORIA");
    send_handshake(conexion_memoria, entradasalida_log, " I/O / MEMORIA");

    conexion_kernel = crear_conexion(ip_kernel, puerto_kernel);
    log_info(entradasalida_log, "I/O conectado a KERNEL");
    send_handshake(conexion_kernel, entradasalida_log, "I/O / KERNEL");
    
    return 0;
}
