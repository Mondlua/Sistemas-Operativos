#include "main.h"

t_log* memoria_log;
t_config* memoria_config;

int main(void) {
    
    char* puerto;
    int memoria_server;
    int tam_pagina;
    int tam_memoria;

    memoria_log = iniciar_logger("memoria.log","memoria");
    memoria_config = iniciar_config("memoria.config");


    /* MEMORIA - Server */

    // Extraer configs

    tam_memoria=config_get_int_value(memoria_config, "TAM_MEMORIA");
    tam_pagina=config_get_int_value(memoria_config, "TAM_PAGINA");
    puerto = config_get_string_value(memoria_config ,"PUERTO_ESCUCHA");

    // Inicio server 

    memoria_server = iniciar_servidor(puerto, memoria_log);
    log_info(memoria_log, "MEMORIA lista para recibir clientes");

    t_atender_cliente_args* args = malloc(sizeof(t_atender_cliente_args));
    args->log = memoria_log;
    args->c_socket = memoria_server;
    args->server_name = "memoria"; 
    
    pthread_t hilo;
    pthread_create(&hilo, NULL, (void *)server_escuchar, args);
    
    pthread_join(hilo,NULL);
    return 0;
}


