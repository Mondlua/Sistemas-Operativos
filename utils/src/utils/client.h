#ifndef UTILS_CLIENT_H
#define UTILS_CLIENT_H

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/config.h>


int crear_conexion(char* ip, char* puerto);
void liberar_conexion(int socket_cliente);

#endif /* UTILS_H */
