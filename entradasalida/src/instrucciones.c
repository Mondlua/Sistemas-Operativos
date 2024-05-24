#include "instrucciones.h"


instruccion_params* deserializar_io_gen_sleep(t_buffer_ins* buffer)
{
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    memcpy(&(parametros->params.io_gen_sleep_params.unidades_trabajo), buffer->stream, sizeof(int));
    return parametros;
}

instruccion_params* recibir_instruccion(char* tipo_interfaz, int socket_servidor)
{
    t_paquete_instruccion* instruccion = malloc(sizeof(t_paquete_instruccion));
    instruccion->buffer = malloc(sizeof(t_buffer_ins));
    recv(socket_servidor, &(instruccion->codigo_operacion), sizeof(instrucciones), MSG_WAITALL);
    recv(socket_servidor, &(instruccion->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    instruccion->buffer->stream = malloc(instruccion->buffer->size);
    recv(socket_servidor, instruccion->buffer->stream, instruccion->buffer->size, MSG_WAITALL);
    instruccion_params* param;
    if(validar_operacion(tipo_interfaz, instruccion->codigo_operacion)){
        switch (instruccion->codigo_operacion)
    {
        case IO_GEN_SLEEP:
            param = deserializar_io_gen_sleep(instruccion->buffer);
            aviso_segun_cod_op(nombre_interfaz, socket_servidor, AVISO_OPERACION_VALIDADA);
           //meter a una cola de operaciones y hacer signal
            break;
        // OTRAS FUNCIONES
        default:
            printf("Tipo de operación no válido.\n");
            free(instruccion->buffer);
            free(instruccion);
            return NULL;
    }
    }
    else{
        char* error = "ERROR DE OPERACION!!";
        aviso_segun_cod_op(error, socket_servidor, AVISO_OPERACION_INVALIDA);
        param = NULL;
    }
    free(instruccion->buffer);
    free(instruccion);
    return param;
}

int validar_operacion(char* tipo_interfaz, int codigo_operacion){
    int resultado = 0;
    if(strcmp(tipo_interfaz, "GENERICA") == 0 && codigo_operacion == 10){resultado = 1;}
    else if(strcmp(tipo_interfaz, "STDIN") == 0 && codigo_operacion == 11){resultado = 1;}
    else if(strcmp(tipo_interfaz, "STDOUT") == 0 && codigo_operacion == 12){resultado = 1;}
    else if(strcmp(tipo_interfaz, "DIALFS") == 0 && (codigo_operacion > 12 && codigo_operacion < 18)){resultado = 1;}
    return resultado;
}