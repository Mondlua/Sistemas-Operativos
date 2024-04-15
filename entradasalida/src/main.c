#include <stdlib.h>
#include <stdio.h>
#include <utils/client.h>
#include <utils/inicio.h>

int main(void) {
    int conexion_memoria;
    char* ip_memoria;
    char* puerto_memoria;

    char* ip_kernel;
    char* puerto_kernel;
    int conexion_kernel;


	char* puerto_escucha;
    t_log* entradasalida_log;
	t_config* entradasalida_config;

    entradasalida_log = iniciar_logger("entradasalida.log","entradasalida");
    entradasalida_config = iniciar_config("entradasalida.config");
    
	// cliente i/o con kernel
	ip_memoria = config_get_string_value(entradasalida_config,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(entradasalida_config, "PUERTO_MEMORIA");

    ip_kernel= config_get_string_value(entradasalida_config,"IP_KERNEL");
    puerto_kernel = config_get_string_value(entradasalida_config, "PUERTO_KERNEL");

	puerto_escucha = config_get_string_value(entradasalida_config, "PUERTO_ESCUCHA");

	// conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    
    conexion_kernel = crear_conexion(ip_kernel, puerto_kernel);


    return 0;
}

