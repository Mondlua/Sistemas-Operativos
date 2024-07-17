#include "io.h"

int logica_int;

void validar_peticion(instruccion_params* parametros, t_pcb* pcb, int codigo_op) {

    sem_wait(&sem_contador_int);
    int tamanio_lista = list_size(interfaces);

    if (tamanio_lista > 0) {
        interfaz* interfaz_encontrada = buscar_interfaz_por_nombre(parametros->interfaz);
        if (interfaz_encontrada != NULL) {
            sem_wait(&interfaz_encontrada->semaforo_interfaz);
            enviar_instruccion_a_interfaz(interfaz_encontrada, parametros, codigo_op, pcb->pid);
            wait(&habilitacion_io);
            if(logica_int){
                queue_push(interfaz_encontrada->cola_block, pcb);
                pcb->estado = BLOCKED;
            }
            else{
                cambiar_a_cola(pcb, EXIT);
            }
        } else {
            printf("La interfaz '%s' no existe en la lista.\n", parametros->interfaz);
            cambiar_a_cola(pcb, EXIT);
        }
    } else {
        printf("La lista de interfaces está vacía.\n");
    }
    sem_post(&sem_contador_int);
}

interfaz* buscar_interfaz_por_nombre(char* nombre_interfaz) {
    int tamanio_lista = list_size(interfaces);
    interfaz* interfaz_encontrada = NULL;

    for (int i = 0; i < tamanio_lista; i++) {
        interfaz* posible_interfaz = (interfaz*)list_get(interfaces, i);
        if (string_equals_ignore_case(posible_interfaz->nombre_interfaz, nombre_interfaz)) {
            interfaz_encontrada = posible_interfaz;
            break;
        }
    }
    return interfaz_encontrada;
}

void enviar_instruccion_a_interfaz(interfaz* interfaz_destino, instruccion_params* parametros, int codigo_op, uint32_t pid) {
    t_paquete_instruccion* instruccion_enviar = malloc(sizeof(t_paquete_instruccion));

    instruccion_enviar->codigo_operacion = codigo_op;
    enviar_instruccion(instruccion_enviar, parametros, interfaz_destino->socket_interfaz, pid);

    free(parametros);
    free(instruccion_enviar);
}



// RECIBIR DE CPU

instruccion_params* deserializar_io_gen_sleep_con_interfaz(t_buffer_ins* buffer) {
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    
    uint32_t offset = 0;
    uint32_t interfaz_len;
    memcpy(&interfaz_len, buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    parametros->interfaz = malloc(interfaz_len);
    memcpy(parametros->interfaz, buffer->stream + offset, interfaz_len);
    offset += interfaz_len;
    memcpy(&(parametros->params.io_gen_sleep.unidades_trabajo), buffer->stream + offset, sizeof(int));
    
    return parametros;
}

instruccion_params* deserializar_io_stdin_stdout_con_interfaz(t_buffer_ins* buffer) {
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    
    uint32_t offset = 0;
    uint32_t interfaz_len;
    memcpy(&interfaz_len, buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    parametros->interfaz = malloc(interfaz_len);
    memcpy(parametros->interfaz, buffer->stream + offset, interfaz_len);
    offset += interfaz_len;
    memcpy(&(parametros->registro_direccion), buffer->stream + offset, sizeof(t_dir_fisica));
    offset += sizeof(t_dir_fisica);
    memcpy(&(parametros->registro_tamanio), buffer->stream + offset, sizeof(uint32_t));
    
    return parametros;
}

instruccion_params* deserializar_io_fs_create_delete_con_interfaz(t_buffer_ins* buffer) {
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    
    uint32_t offset = 0;
    uint32_t interfaz_len;
    memcpy(&interfaz_len, buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    parametros->interfaz = malloc(interfaz_len);
    memcpy(parametros->interfaz, buffer->stream + offset, interfaz_len);
    offset += interfaz_len;
    
    uint32_t archivo_len;
    memcpy(&archivo_len, buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    parametros->params.io_fs.nombre_archivo = malloc(archivo_len);
    memcpy(parametros->params.io_fs.nombre_archivo, buffer->stream + offset, archivo_len);
    
    return parametros;
}

instruccion_params* deserializar_io_fs_truncate_con_interfaz(t_buffer_ins* buffer) {
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    
    uint32_t offset = 0;
    uint32_t interfaz_len;
    memcpy(&interfaz_len, buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    parametros->interfaz = malloc(interfaz_len);
    memcpy(parametros->interfaz, buffer->stream + offset, interfaz_len);
    offset += interfaz_len;
    
    uint32_t archivo_len;
    memcpy(&archivo_len, buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    parametros->params.io_fs.nombre_archivo = malloc(archivo_len);
    memcpy(parametros->params.io_fs.nombre_archivo, buffer->stream + offset, archivo_len);
    offset += archivo_len;
    
    memcpy(&(parametros->registro_tamanio), buffer->stream + offset, sizeof(uint32_t));
    
    return parametros;
}

instruccion_params* deserializar_io_fs_write_read_con_interfaz(t_buffer_ins* buffer) {
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    
    uint32_t offset = 0;
    uint32_t interfaz_len;
    memcpy(&interfaz_len, buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    parametros->interfaz = malloc(interfaz_len);
    memcpy(parametros->interfaz, buffer->stream + offset, interfaz_len);
    offset += interfaz_len;
    
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


void recibir_solicitud_cpu(int socket_servidor, t_pcb* pcb)
{
    t_paquete_instruccion* instruccion = malloc(sizeof(t_paquete_instruccion));
    instruccion->buffer = malloc(sizeof(t_buffer_ins));
    
    recv(socket_servidor, &(instruccion->codigo_operacion), sizeof(instrucciones), MSG_WAITALL);
    recv(socket_servidor, &(instruccion->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    
    instruccion->buffer->stream = malloc(instruccion->buffer->size);
    
    recv(socket_servidor, instruccion->buffer->stream, instruccion->buffer->size, MSG_WAITALL);
    instruccion_params* param = NULL;
   
    switch (instruccion->codigo_operacion) {
        case IO_GEN_SLEEP:{
            param = deserializar_io_gen_sleep_con_interfaz(instruccion->buffer);
            break;
        }
        case IO_STDIN_READ:
        case IO_STDOUT_WRITE:
            param = deserializar_io_stdin_stdout_con_interfaz(instruccion->buffer);
            break;
        case IO_FS_CREATE:
        case IO_FS_DELETE:
            param = deserializar_io_fs_create_delete_con_interfaz(instruccion->buffer);
            break;
        case IO_FS_TRUNCATE:
            param = deserializar_io_fs_truncate_con_interfaz(instruccion->buffer);
            break;
        case IO_FS_WRITE:
        case IO_FS_READ:
            param = deserializar_io_fs_write_read_con_interfaz(instruccion->buffer);
            break;
         // Otros casos
        default:
            printf("Tipo de operación no válido.\n");
            break;
        }
    validar_peticion(param, pcb, instruccion->codigo_operacion);
    free(param->interfaz);
    free(param);
    free(instruccion->buffer->stream);
    free(instruccion->buffer);
    free(instruccion);
}
