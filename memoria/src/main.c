#include <stdlib.h>
#include <stdio.h>
#include <utils/server.h>
#include <utils/inicio.h>

int main(void) {
    t_log* memoria_log;
	t_config* memoria_config;
    char* puerto;
    int memoria_server;

    memoria_log = iniciar_logger("memoria.log","memoria");
    memoria_config = iniciar_config("memoria.config");
    puerto = config_get_string_value(puerto ,"PUERTO_ESCUCHA");
    memoria_server = iniciar_servidor(puerto);
    log_info(memoria_log, "Modulo memoria lista para recibir a kernel");
    esperar_cliente(memoria_server);

    return 0;
}
