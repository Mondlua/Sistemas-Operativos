#include "gestion_io.h"

instruccion_params* recibir_registro_direccion_tamanio_con_texto(int client_socket){
    t_paquete_instruccion* instruccion = malloc(sizeof(t_paquete_instruccion));
    instruccion->buffer = malloc(sizeof(t_buffer_ins));
    recv(client_socket, &(instruccion->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    instruccion->buffer->stream = malloc(instruccion->buffer->size);
    recv(client_socket, instruccion->buffer->stream, instruccion->buffer->size, MSG_WAITALL);
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    parametros->registro_direccion = malloc(sizeof(t_dir_fisica));
    void* stream = instruccion->buffer->stream;
    memcpy(&(parametros->registro_direccion->nro_frame), stream, sizeof(int));
    stream += sizeof(int);
    memcpy(&(parametros->registro_direccion->desplazamiento), stream, sizeof(int));
    stream += sizeof(int);
    memcpy(&(parametros->registro_tamanio), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    size_t tamanio_texto = sizeof(parametros->registro_tamanio);
    memcpy(&(parametros->texto), stream, sizeof(tamanio_texto));
    return parametros;
}

instruccion_params* recibir_registro_direccion_tamanio(int client_socket){
    t_paquete_instruccion* instruccion = malloc(sizeof(t_paquete_instruccion));
    instruccion->buffer = malloc(sizeof(t_buffer_ins));
    recv(client_socket, &(instruccion->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    instruccion->buffer->stream = malloc(instruccion->buffer->size);
    recv(client_socket, instruccion->buffer->stream, instruccion->buffer->size, MSG_WAITALL);
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    void* stream = instruccion->buffer->stream;
    memcpy(&(parametros->registro_direccion->nro_frame), stream, sizeof(int));
    stream += sizeof(int);
    memcpy(&(parametros->registro_direccion->desplazamiento), stream, sizeof(int));
    stream += sizeof(int);
    memcpy(&(parametros->registro_tamanio), stream, sizeof(cpu_registros));
    return parametros;
}