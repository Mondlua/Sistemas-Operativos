#include <stdlib.h>
#include <stdio.h>
#include <utils/inicio.h>
#include <utils/comunicacion.h>

int main(void) {

    t_log* memoria_log;
	t_config* memoria_config;

    char* puerto;
    int memoria_server;


    memoria_log = iniciar_logger("memoria.log","memoria");
    memoria_config = iniciar_config("memoria.config");


    /* MEMORIA - Server */

    // Extraer configs

    puerto = config_get_string_value(memoria_config ,"PUERTO_ESCUCHA");

    // Inicio server 

    memoria_server = iniciar_servidor(puerto, memoria_log);
    log_info(memoria_log, "MEMORIA lista para recibir clientes");
    server_escuchar(memoria_log, "memoria", memoria_server);

    return 0;
}

