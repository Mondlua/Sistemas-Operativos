#include "io_operation.h"

void enviar_instruccion(t_paquete_instruccion* instruccion, instruccion_params* parametros ,int socket_cliente)
{
    t_buffer_ins* buffer = NULL;
    // Llama a la función de serialización correspondiente según el tipo de operación
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
    void* a_enviar = malloc(buffer->size + sizeof(IO_OPERATION) + sizeof(uint32_t));
    memcpy(a_enviar + offset, &(instruccion->codigo_operacion), sizeof(IO_OPERATION));
    offset += sizeof(IO_OPERATION);
    memcpy(a_enviar + offset, &(instruccion->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, instruccion->buffer->stream, instruccion->buffer->size);
    int resultado_send = send(socket_cliente, a_enviar, buffer->size + sizeof(IO_OPERATION) + sizeof(uint32_t), MSG_NOSIGNAL);
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
    buffer->size = strlen(param->params.io_gen_sleep_params.unidades_trabajo) + 1; // Longitud de la cadena + 1 para el carácter nulo
    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);
    memcpy(buffer->stream, param->params.io_gen_sleep_params.unidades_trabajo, buffer->size); // Copiar la cadena directamente
    return buffer;
}
