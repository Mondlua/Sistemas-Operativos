#include <stdlib.h>
#include <stdio.h>
#include <utils/server.h>
#include <utils/inicio.h>
#include <utils/comunicacion.h>

int main(void) {
    t_log* memoria_log;
	t_config* memoria_config;
    char* puerto;
    int memoria_server;
 /*                     SERVER                         */
    memoria_log = iniciar_logger("memoria.log","memoria");
    memoria_config = iniciar_config("memoria.config");
    puerto = config_get_string_value(memoria_config ,"PUERTO_ESCUCHA");
    memoria_server = iniciar_servidor(puerto, memoria_log);
    log_info(memoria_log, "Modulo memoria lista para recibir clientes");
    
    server_escuchar(memoria_log, "memoria", memoria_server);

   // handshake_kernel(conexion);
    return 0;

 
}

void handshake_kernel(int fd_conexion){
    size_t bytes;

    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;

    if(fd_conexion != -1){

    bytes = recv(fd_conexion, &handshake, sizeof(int32_t), MSG_WAITALL);
    if (handshake == 1) {
    bytes = send(fd_conexion, &resultOk, sizeof(int32_t), 0);
    } else {
    bytes = send(fd_conexion, &resultError, sizeof(int32_t), 0);
    }

    }
}

