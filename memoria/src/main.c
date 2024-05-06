#include "main.h"

t_log* memoria_log;
t_config* memoria_config;

int main(void) {
    
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

    t_atender_cliente_args* args = malloc(sizeof(t_atender_cliente_args));
    args->log = memoria_log;
    args->c_socket = memoria_server;
    args->server_name = "memoria"; 
    
    pthread_t hilo;
    pthread_create(&hilo, NULL, (void *)server_escuchar, args);//hilo para que no se estanque en el while 1 y siga con la ejecucion del kernel


    /*int tam_memoria=config_get_int_value(memoria_config, "TAM_MEMORIA");
    int tam_pagina=config_get_int_value(memoria_config, "TAM_PAGINA");
    tabla_paginas* tabla=inicializar_tabla(tam_pagina,tam_memoria);
    if (tabla == NULL) {
    log_info(memoria_log,"tabla vacia");
    }
    log_info(memoria_log,"tabla pags: %d", tabla->cant_paginas );

    pagina *pag_actual = tabla->paginas; */
    //log_info(memoria_log,"pagina: %d", pag_actual);
    /*int numdepag = 0;
    while (numdepag < tabla->cant_paginas) {
        log_info(memoria_log,"pag: %d",pag_actual->nro_pagina);
        numdepag++;
        pag_actual = (pagina *)((char*)pag_actual + sizeof(pagina)); 
    }*/
    pthread_join(hilo,NULL);

    return 0;
}


