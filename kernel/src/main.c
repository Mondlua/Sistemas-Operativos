#include <stdlib.h>
#include <stdio.h>
#include <utils/client.h>

t_log* iniciar_logger(char* ruta, char* emisor);
t_config* iniciar_config(char* ruta);

int main(void) {
    int conexion_cpu_dispatch;
    int conexion_cpu_interrupt;
    int conexion_memoria;
    char* ip_memoria;
    char* ip_cpu;
    int* puerto_memoria;
    int* puerto_cpu_dispatch;
    int* puerto_cpu_interrupt;
    t_log* kernel_log;
	t_config* kernel_config;

    kernel_log = iniciar_logger("kernel.log","kernel");
    kernel_config = iniciar_config("kernel.config");
    log_info(kernel_log, "hola");
    return 0;
}

t_log* iniciar_logger(char* ruta, char* emisor)
{
	t_log* nuevo_logger;

	nuevo_logger = log_create(ruta, emisor, true, LOG_LEVEL_INFO);
	if (nuevo_logger == NULL) {
		printf("No se pudo crear el logger");

		exit(1);
	}
	return nuevo_logger;
}

t_config* iniciar_config(char* ruta)
{
	t_config* nuevo_config;

	nuevo_config = config_create(ruta);
	if (nuevo_config == NULL) {
		printf("No se pudo crear el config");

		exit(1);
	}
	return nuevo_config;
}
