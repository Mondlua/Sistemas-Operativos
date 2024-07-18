#include "instrucciones.h"


instruccion_params* deserializar_io_gen_sleep(t_buffer_ins* buffer) {
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    memcpy(&(parametros->params.io_gen_sleep.unidades_trabajo), buffer->stream, sizeof(int));
    return parametros;
}

instruccion_params* deserializar_registro_direccion_tamanio(t_buffer_ins* buffer) {
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    void* stream = buffer->stream;
    memcpy(&(parametros->registro_direccion), stream, sizeof(t_dir_fisica));
    stream += sizeof(t_dir_fisica);
    memcpy(&(parametros->registro_tamanio), stream, sizeof(uint32_t));
    return parametros;
}

instruccion_params* deserializar_io_fs_create_delete(t_buffer_ins* buffer) {
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    
    uint32_t offset = 0;
    uint32_t archivo_len;
    memcpy(&archivo_len, buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    parametros->params.io_fs.nombre_archivo = malloc(archivo_len);
    memcpy(parametros->params.io_fs.nombre_archivo, buffer->stream + offset, archivo_len);
    
    return parametros;
}

instruccion_params* deserializar_io_fs_truncate(t_buffer_ins* buffer) {
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    
    uint32_t offset = 0;
    uint32_t archivo_len;
    memcpy(&archivo_len, buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    parametros->params.io_fs.nombre_archivo = malloc(archivo_len);
    memcpy(parametros->params.io_fs.nombre_archivo, buffer->stream + offset, archivo_len);
    offset += archivo_len;
    memcpy(&(parametros->registro_tamanio), buffer->stream + offset, sizeof(uint32_t));
    
    return parametros;
}

instruccion_params* deserializar_io_fs_write_read(t_buffer_ins* buffer) {
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    
    uint32_t offset = 0;
    uint32_t archivo_len;
    memcpy(&archivo_len, buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    parametros->params.io_fs.nombre_archivo = malloc(archivo_len);
    memcpy(parametros->params.io_fs.nombre_archivo, buffer->stream + offset, archivo_len);
    offset += archivo_len;
    memcpy(&(parametros->registro_direccion), buffer->stream + offset, sizeof(t_dir_fisica));
    offset += sizeof(t_dir_fisica);
    memcpy(&(parametros->registro_tamanio), buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(parametros->params.io_fs.registro_puntero_archivo), buffer->stream + offset, sizeof(off_t));
    
    return parametros;
}

void recibir_instruccion(char* tipo_interfaz)
{
    while (1){
        t_paquete_instruccion* instruccion = malloc(sizeof(t_paquete_instruccion));
        instruccion->buffer = malloc(sizeof(t_buffer_ins));
        uint32_t pid = -1;
        recv(conexion_kernel, &(instruccion->codigo_operacion), sizeof(instrucciones), MSG_WAITALL);
        recv(conexion_kernel, &(pid), sizeof(uint32_t), MSG_WAITALL);
        recv(conexion_kernel, &(instruccion->buffer->size), sizeof(uint32_t), MSG_WAITALL);
        instruccion->buffer->stream = malloc(instruccion->buffer->size);
        recv(conexion_kernel, instruccion->buffer->stream, instruccion->buffer->size, MSG_WAITALL);
        instruccion_params* param;

        if(validar_operacion(tipo_interfaz, instruccion->codigo_operacion)){
            switch (instruccion->codigo_operacion)
            {
            case IO_GEN_SLEEP:{
            param = deserializar_io_gen_sleep(instruccion->buffer);
            break;
            }
            case IO_STDIN_READ:{
            param = deserializar_registro_direccion_tamanio(instruccion->buffer);
            break;
            }
            case IO_STDOUT_WRITE:{
            param = deserializar_registro_direccion_tamanio(instruccion->buffer);
            break;
            }
            case IO_FS_CREATE:
            case IO_FS_DELETE: {
                param = deserializar_io_fs_create_delete(instruccion->buffer);
                break;
            }
            case IO_FS_TRUNCATE: {
                param = deserializar_io_fs_truncate(instruccion->buffer);
                break;
            }
            case IO_FS_WRITE:
            case IO_FS_READ: {
                param = deserializar_io_fs_write_read(instruccion->buffer);
                break;
            }
        // OTRAS FUNCIONES
            default:
            printf("Tipo de operación no válido.\n");
            free(instruccion->buffer);
            free(instruccion);
            continue;
            }
            /*char* logica = "1";
            aviso_segun_cod_op(logica, conexion_kernel, AVISO_OPERACION_VALIDADA);*/
            atender_cod_op(param, instruccion->codigo_operacion);
            free(instruccion->buffer);
            free(instruccion);
            if (param != NULL) {
            free(param);
            }
        }
        else{
            aviso_segun_cod_op(nombre_interfaz, conexion_kernel, AVISO_OPERACION_INVALIDA);
            param = NULL;
            free(instruccion->buffer);
            free(instruccion);
        }
        
    }

}

void atender_cod_op(instruccion_params* parametros, instrucciones op_code){
    switch (op_code)
    {
    case IO_GEN_SLEEP:{
        int result = 0;
        int unidades_trabajo_recibidas = parametros->params.io_gen_sleep.unidades_trabajo;
        result = unidades_trabajo_recibidas * tiempo_unidad_trabajo * 1000; 
        usleep(result);
        break;
    }
    case IO_STDIN_READ:{
        uint32_t tamanio = parametros->registro_tamanio;
        char* texto = (char*)malloc(sizeof(tamanio));
        printf("Ingrese el texto: ");
        fgets(texto, tamanio, stdin);
        parametros->texto = texto;
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
    case IO_FS_CREATE:{
        crear_archivo(parametros->params.io_fs.nombre_archivo);
        break;
    }
    case IO_FS_DELETE:{
        borrar_archivo(parametros->params.io_fs.nombre_archivo);
        break;
    }
    case IO_FS_TRUNCATE:{
        truncar_archivo(parametros->params.io_fs.nombre_archivo, parametros->registro_tamanio);
        break;
    }
    case IO_FS_WRITE:{
        t_paquete_instruccion* instruccion_enviar = malloc(sizeof(t_paquete_instruccion));
        instruccion_enviar->codigo_operacion = IO_FS_WRITE;
        enviar_instruccion_IO_Mem(instruccion_enviar,parametros,conexion_memoria);
        free(instruccion_enviar);
        char* a_escribir = recibir_mensaje(conexion_memoria, entradasalida_log);
        escribir_archivo(parametros->params.io_fs.nombre_archivo, parametros->params.io_fs.registro_puntero_archivo, a_escribir, parametros->registro_tamanio);
        free(a_escribir);
        break;
    }
    case IO_FS_READ:{
        char* leido = leer_archivo(parametros->params.io_fs.nombre_archivo, parametros->params.io_fs.registro_puntero_archivo, parametros->registro_tamanio);
        t_paquete_instruccion* instruccion_enviar = malloc(sizeof(t_paquete_instruccion));
        instruccion_enviar->codigo_operacion = IO_FS_READ;
        parametros->texto = leido;
        enviar_instruccion_IO_Mem(instruccion_enviar,parametros,conexion_memoria);
        free(leido);
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
