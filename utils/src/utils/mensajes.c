#include <utils/mensajes.h>

int recibir_operacion(int socket_cliente)
{
    int cod_op;
    if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
        return cod_op;
    else
    {
        close(socket_cliente);
        return -1;
    }
}

void* recibir_buffer(int* size, int socket_cliente)
{
    void * buffer;
    recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
    recv(socket_cliente, buffer, *size, MSG_WAITALL);
    return buffer;
}

void recibir_mensaje(int socket_cliente, t_log* logger)
{
    int size;
    char* buffer = recibir_buffer(&size, socket_cliente);
    log_info(logger, "Me llego el mensaje %s", buffer);
    free(buffer);
}

char* recibir_interfaz(int socket_cliente, t_log* logger)
{
    int size;
    char* buffer = recibir_buffer(&size, socket_cliente);
    log_info(logger, "Interfaz conectada: %s", buffer);
   return buffer;
}

t_list* recibir_paquete(int socket_cliente)
{
    int size;
    int desplazamiento = 0;
    void * buffer;
    t_list* valores = list_create();
    int tamanio;

    buffer = recibir_buffer(&size, socket_cliente);
    while(desplazamiento < size)
    {
        memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
        desplazamiento+=sizeof(int);
        char* valor = malloc(tamanio);
        memcpy(valor, buffer+desplazamiento, tamanio);
        desplazamiento+=tamanio;
        list_add(valores, valor);
    }
    free(buffer);
    return valores;
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;
	return magic;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

    int resultado_send = send(socket_cliente, a_enviar, bytes, MSG_NOSIGNAL);  // Evita la generación de SIGPIPE

    if (resultado_send == -1) {
        fprintf(stderr, "Error al enviar el mensaje: socket cerrado.\n");
    }

    free(a_enviar);
    eliminar_paquete(paquete);
}

void enviar_interfaz(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = INTERFAZ;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void aviso_desconexion(char* mensaje, int socket_cliente){
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = AVISO_DESCONEXION;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

char* recibir_desconexion(int socket_cliente, t_log* logger)
{
    int size;
    char* buffer = recibir_buffer(&size, socket_cliente);
    log_info(logger, "Interfaz desconectada: %s", buffer);
   return buffer;
}

void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

bool rcv_handshake(int fd_conexion){
    size_t bytes;

    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;

    if(fd_conexion != -1){

        bytes = recv(fd_conexion, &handshake, sizeof(int32_t), MSG_WAITALL);
        if (handshake == 1) {
        bytes = send(fd_conexion, &resultOk, sizeof(int32_t), 0);
        } else {
        bytes = send(fd_conexion, &resultError, sizeof(int32_t), 0);
        }
    }
    return true;
}

bool send_handshake(int conexion, t_log* logger, const char* conexion_name){
    size_t bytes;

    int32_t handshake = 1;
    int32_t result;

    bytes = send(conexion, &handshake, sizeof(int32_t), 0);
    bytes = recv(conexion, &result, sizeof(int32_t), MSG_WAITALL);

    if (result == 0) {
    log_info(logger, "Handshake OK de %s", conexion_name);
    // Handshake OK
    } 
    else {
    // Handshake ERROR
    log_info(logger,"Error handshake de %s", conexion_name); 
    }   
    return true;
}


void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

//INSTRUCCIONES

void enviar_instruccion(t_paquete_instruccion* instruccion, instruccion_params* parametros ,int socket_cliente)
{
    // Llama a la función de serialización correspondiente según el tipo de operación
    switch (instruccion->codigo_operacion)
    {
        case IO_GEN_SLEEP:
            t_buffer_ins* buffer = serializar_io_gen_sleep(parametros);
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
            break;
        // OTRAS INSTRUCCIONES
        default:
            printf("Tipo de operación no válido.\n");
            return;
    }
    
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

instruccion_params* deserializar_io_gen_sleep(t_buffer_ins* buffer)
{
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    parametros->params.io_gen_sleep_params.unidades_trabajo = strdup(buffer->stream); // Asignar la cadena desde el stream
    return parametros;
}

instruccion_params* recibir_instruccion(int socket_servidor)
{
    t_paquete_instruccion* instruccion = malloc(sizeof(t_paquete_instruccion));
    instruccion->buffer = malloc(sizeof(t_buffer_ins));
    recv(socket_servidor, &(instruccion->codigo_operacion), sizeof(IO_OPERATION), MSG_WAITALL);
    recv(socket_servidor, &(instruccion->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    instruccion->buffer->stream = malloc(instruccion->buffer->size);
    recv(socket_servidor, instruccion->buffer->stream, instruccion->buffer->size, MSG_WAITALL);
    instruccion_params* param;
    switch (instruccion->codigo_operacion)
    {
        case IO_GEN_SLEEP:
            param = deserializar_io_gen_sleep(instruccion->buffer);
            break;
        // OTRAS FUNCIONES
        default:
            printf("Tipo de operación no válido.\n");
            free(instruccion);
            return NULL;
    }
    free(instruccion);
    return param;
}