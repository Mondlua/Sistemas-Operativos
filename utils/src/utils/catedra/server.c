#include "server.h"

int iniciar_servidor(char* PUERTO, t_log* logger)
{
    int socket_servidor;

    struct addrinfo hints, *server_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, PUERTO, &hints, &server_info);

    socket_servidor = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

    const int enable = 1;
    if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        return EXIT_FAILURE;
    }

    bind(socket_servidor, server_info->ai_addr, server_info->ai_addrlen);

    listen(socket_servidor, SOMAXCONN);

    log_trace(logger, "Listo para escuchar a mi cliente");
    
    freeaddrinfo(server_info);

    return socket_servidor;
}

int esperar_cliente(int socket_servidor, t_log* logger)
{
    int socket_cliente;

    socket_cliente = accept(socket_servidor, NULL, NULL);
    log_info(logger, "Se conecto un cliente!");

    rcv_handshake(socket_cliente);
    
    return socket_cliente;
}

