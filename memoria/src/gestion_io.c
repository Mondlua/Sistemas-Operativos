#include "gestion_io.h"

instruccion_params* recibir_io_stdin(int client_socket){
    t_paquete_instruccion* instruccion = malloc(sizeof(t_paquete_instruccion));
    instruccion->buffer = malloc(sizeof(t_buffer_ins));
    recv(client_socket, &(instruccion->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    instruccion->buffer->stream = malloc(instruccion->buffer->size);
    recv(client_socket, instruccion->buffer->stream, instruccion->buffer->size, MSG_WAITALL);
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    void* stream = instruccion->buffer->stream;
    memcpy(&(parametros->params.io_stdin_stdout.registro_direccion), stream, sizeof(cpu_registros));
    stream += sizeof(cpu_registros);
    memcpy(&(parametros->params.io_stdin_stdout.registro_tamaño), stream, sizeof(cpu_registros));
    stream += sizeof(cpu_registros);
    size_t tamaño_texto = sizeof(parametros->params.io_stdin_stdout.registro_tamaño);
    memcpy(&(parametros->texto), stream, sizeof(tamaño_texto));
    return parametros;
}

instruccion_params* recibir_io_stdout(int client_socket){
    t_paquete_instruccion* instruccion = malloc(sizeof(t_paquete_instruccion));
    instruccion->buffer = malloc(sizeof(t_buffer_ins));
    recv(client_socket, &(instruccion->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    instruccion->buffer->stream = malloc(instruccion->buffer->size);
    recv(client_socket, instruccion->buffer->stream, instruccion->buffer->size, MSG_WAITALL);
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    void* stream = instruccion->buffer->stream;
    memcpy(&(parametros->params.io_stdin_stdout.registro_direccion), stream, sizeof(cpu_registros));
    stream += sizeof(cpu_registros);
    memcpy(&(parametros->params.io_stdin_stdout.registro_tamaño), stream, sizeof(cpu_registros));
    return parametros;
}