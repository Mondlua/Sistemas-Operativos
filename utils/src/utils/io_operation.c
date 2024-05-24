#include "io_operation.h"

void enviar_instruccion(t_paquete_instruccion* instruccion, instruccion_params* parametros ,int socket_cliente)
{
    t_buffer_ins* buffer = NULL;
    switch (instruccion->codigo_operacion)
    {
        case IO_GEN_SLEEP:
            buffer = serializar_io_gen_sleep(parametros);
            break;
        // OTRAS INSTRUCCIONES
        default:
            printf("Tipo de operación no válido.\n");
            return;
    }
    instruccion->buffer = buffer;
    int offset = 0;
    void* a_enviar = malloc(buffer->size + sizeof(instrucciones) + sizeof(uint32_t));
    memcpy(a_enviar + offset, &(instruccion->codigo_operacion), sizeof(instrucciones));
    offset += sizeof(instrucciones);
    memcpy(a_enviar + offset, &(instruccion->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, instruccion->buffer->stream, instruccion->buffer->size);
    int resultado_send = send(socket_cliente, a_enviar, buffer->size + sizeof(instrucciones) + sizeof(uint32_t), MSG_NOSIGNAL);
    if (resultado_send == -1)
        {
            printf("Error al enviar la instrucción: socket cerrado.\n");
        }
    free(buffer);
    free(a_enviar);
    
}

t_buffer_ins* serializar_io_gen_sleep(instruccion_params* param)
{
    t_buffer_ins* buffer = malloc(sizeof(t_buffer_ins));
    buffer->size = sizeof(int);
    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);
    memcpy(buffer->stream, &(param->params.io_gen_sleep_params.unidades_trabajo), buffer->size); 
    return buffer;
}



//  A KERNEL

t_buffer_ins* serializar_io_gen_sleep_con_interfaz(instruccion_params* param) {
    size_t interfaz_len = strlen(param->interfaz) + 1;
    size_t size = sizeof(uint32_t) + interfaz_len + sizeof(int);
    
    t_buffer_ins* buffer = malloc(sizeof(t_buffer_ins));
    buffer->size = size;
    buffer->stream = malloc(size);
    
    size_t offset = 0;
    memcpy(buffer->stream + offset, &interfaz_len, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer->stream + offset, param->interfaz, interfaz_len);
    offset += interfaz_len;
    memcpy(buffer->stream + offset, &(param->params.io_gen_sleep_params.unidades_trabajo), sizeof(int));
    
    return buffer;
}


void enviar_instruccion_a_Kernel(t_paquete_instruccion* instruccion, instruccion_params* parametros, int socket_cliente) {
    t_buffer_ins* buffer = NULL;
    
    switch (instruccion->codigo_operacion) {
        case IO_GEN_SLEEP:
            buffer = serializar_io_gen_sleep_con_interfaz(parametros);
            break;
        // Otras operaciones de serialización para otros tipos de operaciones
        default:
            printf("Tipo de operación no válido.\n");
            return;
    }
    
    instruccion->buffer = buffer;
    size_t total_size = sizeof(instrucciones) + sizeof(uint32_t) + buffer->size;
    void* a_enviar = malloc(total_size);
    size_t offset = 0;
    
    memcpy(a_enviar + offset, &(instruccion->codigo_operacion), sizeof(instrucciones));
    offset += sizeof(instrucciones);
    memcpy(a_enviar + offset, &(buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, buffer->stream, buffer->size);
    
    int resultado_send = send(socket_cliente, a_enviar, total_size, MSG_NOSIGNAL);
    if (resultado_send == -1) {
        printf("Error al enviar la instrucción: socket cerrado.\n");
    }
    
    free(buffer->stream);
    free(buffer);
    free(a_enviar);
}
