#include <stdlib.h>
#include <stdio.h>
#include <utils/inicio.h>
#include <utils/comunicacion.h>

int main(void) {
    t_log* memoria_log;
	t_config* memoria_config;
    char* puerto;
    int memoria_server;
    
 /*                     SERVER                         */
    memoria_log = iniciar_logger("memoria.log","Memoria");
    memoria_config = iniciar_config("memoria.config");
    puerto = config_get_string_value(memoria_config ,"PUERTO_ESCUCHA");
    memoria_server = iniciar_servidor(puerto, memoria_log);
    log_info(memoria_log, "Modulo Memoria lista para recibir clientes");
    server_escuchar(memoria_log, "Memoria", memoria_server);

    return 0;

}

