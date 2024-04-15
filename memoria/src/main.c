#include <stdlib.h>
#include <stdio.h>
#include <utils/server.h>
#include <utils/inicio.h>
#include <comunicacion.h>

int main(void) {
    t_log* memoria_log;
	t_config* memoria_config;
    char* puerto;
    int memoria_server;
 // servidor 
    memoria_log = iniciar_logger("memoria.log","memoria");
    memoria_config = iniciar_config("memoria.config");
    puerto = config_get_string_value(memoria_config ,"PUERTO_ESCUCHA");
    memoria_server = iniciar_servidor(puerto, memoria_log);
    log_info(memoria_log, "Modulo memoria lista para recibir");
    while (server_escuchar(memoria_log, "memoria", memoria_server));

    //esperar_cliente(memoria_server, memoria_log);


    return 0;
}
