#include <utils/inicio.h>

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
