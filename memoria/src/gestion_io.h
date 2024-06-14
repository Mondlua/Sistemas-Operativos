#ifndef GESTION_IO_H_
#define GESTION_IO_H_

#include <utils/mensajesPropios.h>
#include <utils/io_operation.h>
#include <stdlib.h>
#include <stdio.h>

instruccion_params* recibir_io_stdin(int client_socket);
instruccion_params* recibir_io_stdout(int client_socket);

#endif