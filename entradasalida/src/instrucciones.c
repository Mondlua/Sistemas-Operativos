#include "instrucciones.h"


instruccion_params* deserializar_io_gen_sleep(t_buffer_ins* buffer) {
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    memcpy(&(parametros->params.io_gen_sleep.unidades_trabajo), buffer->stream, sizeof(int));
    return parametros;
}

instruccion_params* deserializar_registro_direccion_tamanio(t_buffer_ins* buffer) {
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    void* stream = buffer->stream;
    memcpy(&(parametros->registro_tamanio), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(parametros->cant_direcciones), stream, sizeof(int));
    stream += sizeof(int);
    parametros->registro_direccion = malloc(sizeof(t_dir_fisica) * parametros->cant_direcciones);
    memcpy(parametros->registro_direccion,stream, sizeof(t_dir_fisica) * parametros->cant_direcciones);
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
    memcpy(&(parametros->registro_tamanio), buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(parametros->cant_direcciones), buffer->stream + offset, sizeof(int));
    offset += sizeof(int);
    parametros->registro_direccion = malloc(sizeof(t_dir_fisica) * parametros->cant_direcciones);
    memcpy(parametros->registro_direccion,buffer->stream + offset, sizeof(t_dir_fisica) * parametros->cant_direcciones);
    offset += parametros->cant_direcciones * sizeof(t_dir_fisica);
    memcpy(&(parametros->params.io_fs.registro_puntero_archivo), buffer->stream + offset, sizeof(off_t));
    
    return parametros;
}

void recibir_instruccion(char* tipo_interfaz)
{
    while (1){
        t_paquete_instruccion* instruccion = malloc(sizeof(t_paquete_instruccion));
        instruccion->buffer = malloc(sizeof(t_buffer_ins));
        uint32_t pid = -1;
        int bytes_recibidos = recv(conexion_kernel, &(instruccion->codigo_operacion), sizeof(instrucciones), MSG_WAITALL);
        if (bytes_recibidos == 0) {
            log_error(log_aux, "El cliente ha cerrado la conexión.\n");
            free(instruccion->buffer);
            free(instruccion);
            free(nombre_interfaz);
            if(strcmp(tipo_interfaz, "DIALFS") == 0){
                bitarray_destroy(bitmap);
                list_destroy_and_destroy_elements(lista_archivos, destruir_file);
            }
            config_destroy(entradasalida_config);
            log_destroy(log_aux);
            log_destroy(entradasalida_log);
            return;
        } 
        recv(conexion_kernel, &(pid), sizeof(uint32_t), MSG_WAITALL);
        recv(conexion_kernel, &(instruccion->buffer->size), sizeof(uint32_t), MSG_WAITALL);
        instruccion->buffer->stream = malloc(instruccion->buffer->size);
        if (instruccion->buffer->stream == NULL) {
            log_error(log_aux, "Error al asignar memoria para stream");
            free(instruccion->buffer);
            free(instruccion);
            exit(EXIT_FAILURE);
        }
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
            case IO_FS_CREATE: {
                param = deserializar_io_fs_create_delete(instruccion->buffer);
                break;
            }
            case IO_FS_DELETE: {
                param = deserializar_io_fs_create_delete(instruccion->buffer);
                break;
            }
            case IO_FS_TRUNCATE: {
                param = deserializar_io_fs_truncate(instruccion->buffer);
                break;
            }
            case IO_FS_WRITE: {
                param = deserializar_io_fs_write_read(instruccion->buffer);
                break;
            }
            case IO_FS_READ: {
                param = deserializar_io_fs_write_read(instruccion->buffer);
                break;
            }
        // OTRAS FUNCIONES
            default:
            printf("Tipo de operación no válido.\n");
            free(instruccion->buffer->stream);
            free(instruccion->buffer);
            free(instruccion);
            continue;
            }
            atender_cod_op(param, instruccion->codigo_operacion, pid);
            free(instruccion->buffer->stream);
            free(instruccion->buffer);
            free(instruccion);
            aviso_segun_cod_op(nombre_interfaz, conexion_kernel,AVISO_OPERACION_FINALIZADA);
        }
        else{
            aviso_segun_cod_op(nombre_interfaz, conexion_kernel, AVISO_OPERACION_INVALIDA);
            param = NULL;
            free(instruccion->buffer->stream);
            free(instruccion->buffer);
            free(instruccion);
        }
    }

}

void atender_cod_op(instruccion_params* parametros, instrucciones op_code, uint32_t pid){
    log_info(entradasalida_log, "PID: %i - Operacion: %s", pid, op_code_a_string(op_code));
    switch (op_code)
    {
    case IO_GEN_SLEEP:{
        int result = 0;
        int unidades_trabajo_recibidas = parametros->params.io_gen_sleep.unidades_trabajo;
        result = unidades_trabajo_recibidas * tiempo_unidad_trabajo * 1000; 
        usleep(result);
        log_debug(log_aux, "Termine de dormir %dms", result);
        break;
    }
    case IO_STDIN_READ:{
        uint32_t tamanio = parametros->registro_tamanio;
        char* prompt = (char*)malloc(256);
        if (prompt == NULL) {
            log_error(log_aux, "Error al asignar memoria para el prompt");
            break;
        }   
        log_debug(log_aux, "Ingrese el texto de tamaño %i: ", tamanio);
        snprintf(prompt, 256, " >> ");
        char* texto = readline(prompt);
        free(prompt); 
        if (texto == NULL) {
            log_error(log_aux, "Error al leer el texto");
            break;
        }
        if (strlen(texto) > tamanio) {
            texto[tamanio] = '\0'; // Truncar el texto al tamaño permitido
        }
        parametros->texto = texto;
        t_paquete_instruccion* instruccion_enviar = malloc(sizeof(t_paquete_instruccion));
        instruccion_enviar->codigo_operacion = READ_IO;
        enviar_instruccion_IO_Mem(instruccion_enviar,parametros,conexion_memoria, pid);
        free(texto);
        free(instruccion_enviar);
        break;
    }
    case IO_STDOUT_WRITE:{
        t_paquete_instruccion* instruccion_enviar = malloc(sizeof(t_paquete_instruccion));
        instruccion_enviar->codigo_operacion = WRITE_IO;
        enviar_instruccion_IO_Mem(instruccion_enviar,parametros,conexion_memoria, pid);
        free(instruccion_enviar);
        int nousar= recibir_operacion(conexion_memoria);
        char* imprimir = recibir_mensaje(conexion_memoria, entradasalida_log);
        if(imprimir == NULL){
            log_error(log_aux, "Error al recibir el mensaje!");
            break;
        }
        log_debug(log_aux, " >> %s", imprimir);
        free(imprimir);
        break;
    }
    case IO_FS_CREATE:{
        log_info(entradasalida_log, "PID: %i - Crear Archivo: %s", pid,  parametros->params.io_fs.nombre_archivo);
        crear_archivo(parametros->params.io_fs.nombre_archivo);
        break;
    }
    case IO_FS_DELETE:{
        log_info(entradasalida_log, "PID: %i - Eliminar Archivo: %s", pid,  parametros->params.io_fs.nombre_archivo);
        borrar_archivo(parametros->params.io_fs.nombre_archivo);
        break;
    }
    case IO_FS_TRUNCATE:{
        log_info(entradasalida_log, "PID: %i - Truncar Archivo: %s - Tamaño: %i", pid,  parametros->params.io_fs.nombre_archivo, parametros->registro_tamanio);
        truncar_archivo(parametros->params.io_fs.nombre_archivo, parametros->registro_tamanio, pid);
        break;
    }
    case IO_FS_WRITE:{
        log_info(entradasalida_log, "PID: %i - Escribir Archivo: %s - Tamaño a Escribir: %i - Puntero Archivo: %jd", pid,  parametros->params.io_fs.nombre_archivo, parametros->registro_tamanio, (intmax_t)parametros->params.io_fs.registro_puntero_archivo);
        t_paquete_instruccion* instruccion_enviar = malloc(sizeof(t_paquete_instruccion));
        instruccion_enviar->codigo_operacion = WRITE_IO_FS;
        enviar_instruccion_IO_Mem(instruccion_enviar,parametros,conexion_memoria, pid);
        free(instruccion_enviar);
        int nousar= recibir_operacion(conexion_memoria);
        char* a_escribir = recibir_mensaje(conexion_memoria, entradasalida_log);
        if(a_escribir == NULL){
            log_error(log_aux, "Error al recibir el mensaje!");
            break;
        }
        escribir_archivo(parametros->params.io_fs.nombre_archivo, parametros->params.io_fs.registro_puntero_archivo, a_escribir, parametros->registro_tamanio);
        free(a_escribir);
        break;
    }
    case IO_FS_READ:{
        log_info(entradasalida_log, "PID: %i - Leer Archivo: %s - Tamaño a Leer: %i - Puntero Archivo: %jd", pid,  parametros->params.io_fs.nombre_archivo, parametros->registro_tamanio, (intmax_t)parametros->params.io_fs.registro_puntero_archivo);
        char* leido = leer_archivo(parametros->params.io_fs.nombre_archivo, parametros->params.io_fs.registro_puntero_archivo, parametros->registro_tamanio);
        if (leido == NULL) {
            log_error(log_aux, "Error al leer el archivo %s", parametros->params.io_fs.nombre_archivo);
            break;
        }
        t_paquete_instruccion* instruccion_enviar = malloc(sizeof(t_paquete_instruccion));
        instruccion_enviar->codigo_operacion = READ_IO_FS;
        parametros->texto = leido;
        enviar_instruccion_IO_Mem(instruccion_enviar,parametros,conexion_memoria, pid);
        free(instruccion_enviar);
        free(leido);
        break;
    }
    default:
        log_error(log_aux, "Operación no válida");
        break;
    }
    // Se liberan recursos dependiendo del tipo de operación
    switch (op_code) {
        case IO_GEN_SLEEP: break;
        case IO_STDIN_READ:{free(parametros->registro_direccion); break;}
        case IO_STDOUT_WRITE:{free(parametros->registro_direccion); break;}
        case IO_FS_CREATE:{free(parametros->params.io_fs.nombre_archivo); break;}
        case IO_FS_DELETE:{free(parametros->params.io_fs.nombre_archivo); break;}
        case IO_FS_TRUNCATE:{free(parametros->params.io_fs.nombre_archivo); break;}
        case IO_FS_WRITE:{free(parametros->params.io_fs.nombre_archivo); free(parametros->registro_direccion); break;}
        case IO_FS_READ:{free(parametros->params.io_fs.nombre_archivo); free(parametros->registro_direccion); break;}
        default: break;
    }
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

const char* op_code_a_string(instrucciones op_code){
    switch (op_code) {
        case IO_GEN_SLEEP: return "IO_GEN_SLEEP";
        case IO_STDIN_READ: return "IO_STDIN_READ";
        case IO_STDOUT_WRITE: return "IO_STDOUT_WRITE";
        case IO_FS_CREATE: return "IO_FS_CREATE";
        case IO_FS_DELETE: return "IO_FS_DELETE";
        case IO_FS_TRUNCATE: return "IO_FS_TRUNCATE";
        case IO_FS_WRITE: return "IO_FS_WRITE";
        case IO_FS_READ: return "IO_FS_READ";
        default: return "UNKNOWN";
    }
}

