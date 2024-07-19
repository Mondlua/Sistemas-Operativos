#include "io_operation.h"


//  DE KERNEL A IO
void enviar_instruccion(t_paquete_instruccion* instruccion, instruccion_params* parametros ,int socket_cliente, uint32_t pid)
{
    t_buffer_ins* buffer = NULL;
    switch (instruccion->codigo_operacion)
    {
        case IO_GEN_SLEEP:
        {
            buffer = serializar_io_gen_sleep(parametros);
            break;
        }
        case IO_STDIN_READ:
        {
            buffer = serializar_registro_direccion_tamanio(parametros);
            break;
        }
        case IO_STDOUT_WRITE:
        {
            buffer = serializar_registro_direccion_tamanio(parametros);
            break;
        }
        case IO_FS_CREATE:
        {
            buffer = serializar_io_fs_create_delete(parametros);
            break;
        }
        case IO_FS_DELETE:
        {
            buffer = serializar_io_fs_create_delete(parametros);
            break;
        }
        case IO_FS_TRUNCATE:
        {
            buffer = serializar_io_fs_truncate_con_interfaz(parametros);
            break;
        }
        case IO_FS_WRITE:
        {
            buffer = serializar_io_fs_write_read_con_interfaz(parametros);
            break;
        }
        case IO_FS_READ:
        {
            buffer = serializar_io_fs_write_read_con_interfaz(parametros);
            break;
        }
        // OTRAS INSTRUCCIONES
        default:{
            printf("Tipo de operación no válido.\n");
            return;
        }
    }
    instruccion->buffer = buffer;
    int offset = 0;
    void* a_enviar = malloc(buffer->size + sizeof(instrucciones) + sizeof(uint32_t)*2);
    memcpy(a_enviar + offset, &(instruccion->codigo_operacion), sizeof(instrucciones));
    offset += sizeof(instrucciones);
    memcpy(a_enviar + offset, &(pid), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, &(instruccion->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, instruccion->buffer->stream, instruccion->buffer->size);
    int resultado_send = send(socket_cliente, a_enviar, buffer->size + sizeof(instrucciones) + sizeof(uint32_t)*2, MSG_NOSIGNAL);
    if (resultado_send == -1)
        {
            printf("Error al enviar la instrucción: socket cerrado.\n");
        }
    free(buffer->stream);
    free(buffer);
    free(a_enviar);
    
}

t_buffer_ins* serializar_io_gen_sleep(instruccion_params* param) {
    t_buffer_ins* buffer = malloc(sizeof(t_buffer_ins));
    buffer->size = sizeof(int);
    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);
    memcpy(buffer->stream, &(param->params.io_gen_sleep.unidades_trabajo), buffer->size); 
    return buffer;
}

t_buffer_ins* serializar_registro_direccion_tamanio(instruccion_params* param) {
    size_t size = sizeof(uint32_t) + sizeof(t_dir_fisica);
    t_buffer_ins* buffer = malloc(sizeof(t_buffer_ins));
    buffer->size = size;
    buffer->stream = malloc(size);
    size_t offset = 0;
    memcpy(buffer->stream + offset, param->registro_direccion, sizeof(t_dir_fisica));
    offset += sizeof(t_dir_fisica);
    memcpy(buffer->stream + offset, &(param->registro_tamanio), sizeof(uint32_t));
    return buffer;
}

t_buffer_ins* serializar_io_fs_create_delete(instruccion_params* param) {
    size_t archivo_len = strlen(param->params.io_fs.nombre_archivo) + 1;
    t_buffer_ins* buffer = malloc(sizeof(t_buffer_ins));
    buffer->size = archivo_len + sizeof(uint32_t);
    buffer->offset = 0;
    size_t offset = buffer->offset;
    buffer->stream = malloc(buffer->size);
    memcpy(buffer->stream + offset, &archivo_len, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer->stream + offset, param->params.io_fs.nombre_archivo, archivo_len);
    return buffer;
}

t_buffer_ins* serializar_io_fs_truncate(instruccion_params* param) {
    t_buffer_ins* buffer = malloc(sizeof(t_buffer_ins));
    size_t archivo_len = strlen(param->params.io_fs.nombre_archivo) + 1;
    buffer->size = sizeof(uint32_t) + archivo_len + sizeof(uint32_t);
    buffer->offset = 0;
    size_t offset = buffer->offset;
    buffer->stream = malloc(buffer->size);
    memcpy(buffer->stream + offset, &archivo_len, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer->stream + offset, param->params.io_fs.nombre_archivo, archivo_len);
    offset += archivo_len;
    memcpy(buffer->stream + offset, &(param->registro_tamanio), sizeof(uint32_t));
    return buffer;
}

t_buffer_ins* serializar_io_fs_write_read(instruccion_params* param) {
    size_t archivo_len = strlen(param->params.io_fs.nombre_archivo) + 1;
    size_t size = sizeof(uint32_t) + archivo_len + sizeof(t_dir_fisica) + sizeof(uint32_t) + sizeof(off_t);
    t_buffer_ins* buffer = malloc(sizeof(t_buffer_ins));
    buffer->size = size;
    buffer->stream = malloc(size);
    size_t offset = 0;
    memcpy(buffer->stream + offset, &archivo_len, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer->stream + offset, param->params.io_fs.nombre_archivo, archivo_len);
    offset += archivo_len;
    memcpy(buffer->stream + offset, param->registro_direccion, sizeof(t_dir_fisica));
    offset += sizeof(t_dir_fisica);
    memcpy(buffer->stream + offset, &(param->registro_tamanio), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer->stream + offset, &(param->params.io_fs.registro_puntero_archivo), sizeof(off_t));
    return buffer;
}


//  DE CPU A KERNEL 

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
    memcpy(buffer->stream + offset, &(param->params.io_gen_sleep.unidades_trabajo), sizeof(int));
    return buffer;
}

t_buffer_ins* serializar_registro_direccion_tamanio_con_interfaz(instruccion_params* param) {
    size_t interfaz_len = strlen(param->interfaz) + 1;
    size_t size = sizeof(uint32_t) + interfaz_len + sizeof(uint32_t) + sizeof(t_dir_fisica);
    t_buffer_ins* buffer = malloc(sizeof(t_buffer_ins));
    buffer->size = size;
    buffer->stream = malloc(size);
    size_t offset = 0;
    memcpy(buffer->stream + offset, &interfaz_len, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer->stream + offset, param->interfaz, interfaz_len);
    offset += interfaz_len;
    memcpy(buffer->stream + offset, param->registro_direccion, sizeof(t_dir_fisica));
    offset += sizeof(t_dir_fisica);
    memcpy(buffer->stream + offset, &(param->registro_tamanio), sizeof(uint32_t));
    return buffer;
}

t_buffer_ins* serializar_io_fs_create_delete_con_interfaz(instruccion_params* param) {
    size_t archivo_len = strlen(param->params.io_fs.nombre_archivo) + 1;
    size_t interfaz_len = strlen(param->interfaz) + 1;
    t_buffer_ins* buffer = malloc(sizeof(t_buffer_ins));
    buffer->size = interfaz_len + archivo_len + 2 * sizeof(uint32_t);
    buffer->offset = 0;
    size_t offset = buffer->offset;
    buffer->stream = malloc(buffer->size);
    memcpy(buffer->stream + offset, &interfaz_len, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer->stream + offset, param->interfaz, interfaz_len);
    offset += interfaz_len;
    memcpy(buffer->stream + offset, &archivo_len, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer->stream + offset, param->params.io_fs.nombre_archivo, archivo_len);
    return buffer;
}

t_buffer_ins* serializar_io_fs_truncate_con_interfaz(instruccion_params* param) {
    size_t archivo_len = strlen(param->params.io_fs.nombre_archivo) + 1;
    size_t interfaz_len = strlen(param->interfaz) + 1;
    t_buffer_ins* buffer = malloc(sizeof(t_buffer_ins));
    buffer->size = interfaz_len + archivo_len + 3 * sizeof(uint32_t);
    buffer->offset = 0;
    size_t offset = buffer->offset;
    buffer->stream = malloc(buffer->size);
    memcpy(buffer->stream + offset, &interfaz_len, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer->stream + offset, param->interfaz, interfaz_len);
    offset += interfaz_len;
    memcpy(buffer->stream + offset, &archivo_len, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer->stream + offset, param->params.io_fs.nombre_archivo, archivo_len);
    offset += archivo_len;
    memcpy(buffer->stream + offset, &(param->registro_tamanio), sizeof(uint32_t));
    return buffer;
}

t_buffer_ins* serializar_io_fs_write_read_con_interfaz(instruccion_params* param) {
    size_t archivo_len = strlen(param->params.io_fs.nombre_archivo) + 1;
    size_t interfaz_len = strlen(param->interfaz) + 1;
    size_t size = sizeof(uint32_t) + interfaz_len + sizeof(uint32_t) + archivo_len + sizeof(t_dir_fisica) + sizeof(uint32_t) + sizeof(off_t);
    t_buffer_ins* buffer = malloc(sizeof(t_buffer_ins));
    buffer->size = size;
    buffer->stream = malloc(size);
    size_t offset = 0;
    memcpy(buffer->stream + offset, &interfaz_len, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer->stream + offset, param->interfaz, interfaz_len);
    offset += interfaz_len;
    memcpy(buffer->stream + offset, &archivo_len, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer->stream + offset, param->params.io_fs.nombre_archivo, archivo_len);
    offset += archivo_len;
    memcpy(buffer->stream + offset, param->registro_direccion, sizeof(t_dir_fisica));
    offset += sizeof(t_dir_fisica);
    memcpy(buffer->stream + offset, &(param->registro_tamanio), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer->stream + offset, &(param->params.io_fs.registro_puntero_archivo), sizeof(off_t));
    return buffer;
}


void enviar_instruccion_a_Kernel(t_paquete_instruccion* instruccion, instruccion_params* parametros, int socket_cliente) {
    t_buffer_ins* buffer = NULL;
    
    switch (instruccion->codigo_operacion) {
         case IO_GEN_SLEEP:
        {
            buffer = serializar_io_gen_sleep_con_interfaz(parametros);
            break;
        }
        case IO_STDIN_READ:
        {
            buffer = serializar_registro_direccion_tamanio_con_interfaz(parametros);
            break;
        }
        case IO_STDOUT_WRITE:
        {
            buffer = serializar_registro_direccion_tamanio_con_interfaz(parametros);
            break;
        }
        case IO_FS_CREATE:
        {
            buffer = serializar_io_fs_create_delete_con_interfaz(parametros);
            break;
        }
        case IO_FS_DELETE:
        {
            buffer = serializar_io_fs_create_delete_con_interfaz(parametros);
            break;
        }
        case IO_FS_TRUNCATE:
        {
            buffer = serializar_io_fs_truncate_con_interfaz(parametros);
            break;
        }
        case IO_FS_WRITE:
        {
            buffer = serializar_io_fs_write_read_con_interfaz(parametros);
            break;
        }
        case IO_FS_READ:
        {
            buffer = serializar_io_fs_write_read_con_interfaz(parametros);
            break;
        }
           
        // Otras operaciones de serialización para otros tipos de operaciones
        default:{
            printf("Tipo de operación no válido.\n");
            return;
        }
    }
    
    instruccion->buffer = buffer;
    size_t total_size = sizeof(int) + sizeof(uint32_t) + buffer->size;
    void* a_enviar = malloc(total_size);
    size_t offset = 0;
    
    memcpy(a_enviar + offset, &(instruccion->codigo_operacion), sizeof(int));
    offset += sizeof(int);
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

//  DE IO A MEMORIA


void enviar_instruccion_IO_Mem(t_paquete_instruccion* instruccion, instruccion_params* parametros, int socket_cliente, uint32_t pid) {
    t_buffer_ins* buffer = NULL;
    
    switch (instruccion->codigo_operacion) {
        case IO_STDIN_READ:{
            buffer = serializar_registro_direccion_tamanio_con_texto(parametros);
            break;
        }
        case IO_STDOUT_WRITE:{
            buffer = serializar_registro_direccion_tamanio(parametros);
            break;
        }
        case IO_FS_WRITE:{
            buffer = serializar_registro_direccion_tamanio(parametros);
            break;
        }
        case IO_FS_READ:{
            buffer = serializar_registro_direccion_tamanio_con_texto(parametros);
            break;
        }
        default:{
            printf("Tipo de operación no válido.\n");
            return;
        }
    }
    
    instruccion->buffer = buffer;
    size_t total_size = sizeof(instrucciones) + sizeof(uint32_t) *2 + buffer->size;
    void* a_enviar = malloc(total_size); 
    size_t offset = 0;
    
    memcpy(a_enviar + offset, &(instruccion->codigo_operacion), sizeof(instrucciones));
    offset += sizeof(instrucciones);
    memcpy(a_enviar + offset, &(pid), sizeof(uint32_t));
    offset += sizeof(uint32_t);
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

t_buffer_ins* serializar_registro_direccion_tamanio_con_texto(instruccion_params* param){
    size_t tamaño_texto = sizeof(param->registro_tamanio);
    size_t size = sizeof(uint32_t) + sizeof(t_dir_fisica) + tamaño_texto;
    t_buffer_ins* buffer = malloc(sizeof(t_buffer_ins));
    buffer->size = size;
    buffer->stream = malloc(size);
    size_t offset = 0;
    memcpy(buffer->stream + offset, &(param->registro_direccion), sizeof(t_dir_fisica));
    offset += sizeof(t_dir_fisica);
    memcpy(buffer->stream + offset, &(param->registro_tamanio), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer->stream + offset, &(param->texto), tamaño_texto);
    
    return buffer;
}

