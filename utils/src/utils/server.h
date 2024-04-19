#ifndef UTILS_SERVER_H_
#define UTILS_SERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<string.h>
#include<assert.h>

int iniciar_servidor(char*, t_log*);
int esperar_cliente(int, t_log*);

#endif