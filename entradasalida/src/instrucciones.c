#include "instrucciones.h"


instruccion_params* deserializar_io_gen_sleep(t_buffer_ins* buffer)
{
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    memcpy(&(parametros->params.io_gen_sleep_params.unidades_trabajo), buffer->stream, sizeof(int));
    return parametros;
}

instruccion_params* deserializar_io_stdin_stdout(t_buffer_ins* buffer){
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    void* stream = buffer->stream;
    memcpy(&(parametros->params.io_stdin_stdout.registro_direccion), stream, sizeof(t_dir_fisica));
    stream += sizeof(t_dir_fisica);
    memcpy(&(parametros->params.io_stdin_stdout.registro_tamaño), stream, sizeof(cpu_registros));
    return parametros;
}

void recibir_instruccion(char* tipo_interfaz)
{
    t_paquete_instruccion* instruccion = malloc(sizeof(t_paquete_instruccion));
    instruccion->buffer = malloc(sizeof(t_buffer_ins));
    recv(conexion_kernel, &(instruccion->codigo_operacion), sizeof(instrucciones), MSG_WAITALL);
    recv(conexion_kernel, &(instruccion->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    instruccion->buffer->stream = malloc(instruccion->buffer->size);
    recv(conexion_kernel, instruccion->buffer->stream, instruccion->buffer->size, MSG_WAITALL);
    instruccion_params* param;
    if(validar_operacion(tipo_interfaz, instruccion->codigo_operacion)){
        switch (instruccion->codigo_operacion)
    {
        case IO_GEN_SLEEP:{
            param = deserializar_io_gen_sleep(instruccion->buffer);
            char* logica = "1";
            aviso_segun_cod_op(logica, conexion_kernel, AVISO_OPERACION_VALIDADA);
            break;
        }
        case IO_STDIN_READ:{
            param = deserializar_io_stdin_stdout(instruccion->buffer);
            char* logica = "1";
            aviso_segun_cod_op(logica, conexion_kernel, AVISO_OPERACION_VALIDADA);
            break;
        }
        case IO_STDOUT_WRITE:{
            param = deserializar_io_stdin_stdout(instruccion->buffer);
            char* logica = "1";
            aviso_segun_cod_op(logica, conexion_kernel, AVISO_OPERACION_VALIDADA);
            break;
        }

        // OTRAS FUNCIONES
        default:
            printf("Tipo de operación no válido.\n");
            free(instruccion->buffer);
            free(instruccion);
    }
    }
    else{
        char* error = "0";
        aviso_segun_cod_op(error, conexion_kernel, AVISO_OPERACION_INVALIDA);
        param = NULL;
    }
    atender_cod_op(param, instruccion->codigo_operacion);
    free(instruccion->buffer);
    free(instruccion);
    free(param);
}

void atender_cod_op(instruccion_params* parametros, instrucciones op_code){
    switch (op_code)
    {
    case IO_GEN_SLEEP:{
        int result = 0;
        int unidades_trabajo_recibidas = parametros->params.io_gen_sleep_params.unidades_trabajo;
        result = unidades_trabajo_recibidas * tiempo_unidad_trabajo; 
        sleep(result);
        break;
    }
    case IO_STDIN_READ:{
        cpu_registros* tamaño = parametros->params.io_stdin_stdout.registro_tamaño;
        char* texto = (char*)malloc(sizeof(tamaño));
        printf("Ingrese el texto: ");
        fgets(texto, tamaño, stdin);
        t_paquete_instruccion* instruccion_enviar = malloc(sizeof(t_paquete_instruccion));
        instruccion_enviar->codigo_operacion = IO_STDIN_READ;
        enviar_instruccion_IO_Mem(instruccion_enviar,parametros,conexion_memoria);
        free(texto);
        free(instruccion_enviar);
        break;
    }
    case IO_STDOUT_WRITE:{
        t_paquete_instruccion* instruccion_enviar = malloc(sizeof(t_paquete_instruccion));
        instruccion_enviar->codigo_operacion = IO_STDOUT_WRITE;
        enviar_instruccion_IO_Mem(instruccion_enviar,parametros,conexion_memoria);
        free(instruccion_enviar);
        char* imprimir = recibir_mensaje(conexion_memoria, entradasalida_log);
        free(imprimir);
        break;
    }
    default:
        break;
    }
    aviso_segun_cod_op(nombre_interfaz, conexion_kernel,AVISO_OPERACION_FINALIZADA);
    free(parametros);
}

int validar_operacion(char* tipo_interfaz, int codigo_operacion){
    int resultado = 0;
    if(strcmp(tipo_interfaz, "GENERICA") == 0 && codigo_operacion == 10){resultado = 1;}
    else if(strcmp(tipo_interfaz, "STDIN") == 0 && codigo_operacion == 11){resultado = 1;}
    else if(strcmp(tipo_interfaz, "STDOUT") == 0 && codigo_operacion == 12){resultado = 1;}
    else if(strcmp(tipo_interfaz, "DIALFS") == 0 && (codigo_operacion > 12 && codigo_operacion < 18)){resultado = 1;}
    return resultado;
}