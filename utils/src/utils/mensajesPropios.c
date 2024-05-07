#include "mensajesPropios.h"

/////////////////////////////* INTERFACES *//////////////////////////////////////

char* recibir_interfaz(int socket_cliente, t_log* logger)
{
    int size;
    char* buffer = recibir_buffer(&size, socket_cliente);
    log_info(logger, "Interfaz conectada: %s", buffer);
   return buffer;
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

//////////////////////////////* INSTRUCCIONES IO *//////////////////////////////////////

void enviar_instruccion(t_paquete_instruccion* instruccion, instruccion_params* parametros ,int socket_cliente)
{
    t_buffer_ins* buffer = NULL;
    // Llama a la función de serialización correspondiente según el tipo de operación
    switch (instruccion->codigo_operacion)
    {
        case IO_GEN_SLEEP:
            buffer = serializar_io_gen_sleep(parametros);
            break;
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

instruccion_params* deserializar_io_gen_sleep(t_buffer_ins* buffer)
{
    instruccion_params* parametros = malloc(sizeof(instruccion_params));
    parametros->params.io_gen_sleep_params.unidades_trabajo = strdup(buffer->stream); // Asignar la cadena desde el stream
    return parametros;
}

//////////////////////////////* INSTRUCCIONES MEMORIA *//////////////////////////////////////

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
        default:
            printf("Tipo de operación no válido.\n");
            free(instruccion->buffer);
            free(instruccion);
            return NULL;
    }
    free(instruccion->buffer);
    free(instruccion);
    return param;
}


void enviar_instruccion_mem(int socket_cliente, t_instruccion* instruccion){
    
    t_buffer_ins* buffer=malloc(sizeof(t_buffer_ins)) ;
    buffer = serializar_instruccion(instruccion);
     
    instruccion->buffer=buffer;
    int offset = 0;
    void* a_enviar = malloc(buffer->size + sizeof(op_code) + sizeof(uint32_t));
    memcpy(a_enviar + offset, &(instruccion->codigo_operacion), sizeof(op_code));
    offset += sizeof(op_code);
    memcpy(a_enviar + offset, &(instruccion->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, instruccion->buffer->stream, instruccion->buffer->size);
    int resultado_send = send(socket_cliente, a_enviar, buffer->size + sizeof(op_code) + sizeof(uint32_t), MSG_NOSIGNAL);
    if (resultado_send == -1)
        {
            printf("Error al enviar la instrucción: socket cerrado.\n");
        }
    free(buffer);
    free(a_enviar);
    }

t_buffer_ins* serializar_instruccion(t_instruccion* ins){
    
    t_buffer_ins* buffer = malloc(sizeof(t_buffer_ins));
    buffer->size = ins->buffer->size ;//strlen(ins->buffer->stream) + 1; // Longitud de la cadena + 1 para el carácter nulo
    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);
    memcpy(buffer->stream, ins->buffer->stream, buffer->size); // Copiar la cadena directamente

    return buffer;
}

t_instruccion* recibir_instruccion_cpu(int socket_servidor){

    t_instruccion* instruccion = malloc(sizeof(t_instruccion));

    instruccion->buffer = malloc(sizeof(t_buffer_ins));

    recv(socket_servidor, &(instruccion->codigo_operacion), sizeof(op_code), MSG_WAITALL);

    recv(socket_servidor, &(instruccion->buffer->size), sizeof(uint32_t), MSG_WAITALL);

    instruccion->buffer->stream = malloc(sizeof(instruccion->buffer->size));
    recv(socket_servidor, instruccion->buffer->stream, instruccion->buffer->size, MSG_WAITALL);
 
    return instruccion;  
    free(instruccion->buffer);
    free(instruccion);
}

//////////////////////////////* PCB *//////////////////////////////////////

void enviar_pc(char* pc, int socket_cliente){

    t_paquete* paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = PC;
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = strlen(pc)+1;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, pc, paquete->buffer->size);

    int bytes = paquete->buffer->size + 2*sizeof(int);

    void* a_enviar = serializar_paquete(paquete, bytes);

    int resultado_send = send(socket_cliente, a_enviar, bytes, MSG_NOSIGNAL);  // Evita la generación de SIGPIPE

    if (resultado_send == -1) {
        fprintf(stderr, "Error al enviar el program counter: socket cerrado.\n");
    }

    free(a_enviar);
    eliminar_paquete(paquete);
}

char* recibir_pc(int socket_cliente){

    int size;
    char* buffer = recibir_buffer(&size, socket_cliente);

    if(buffer==NULL){
        printf("No se recibio nada\n");
    }
    else{
        printf("Me llego el mensaje %s\n", buffer);
    }
    return buffer;
    free(buffer);
}

char *estado_a_string(t_proceso_estado estado)
{
    switch (estado)
    {
    case 0:
        return "NEW";
    case 1:
        return "READY";
    case 2:
        return "EXEC";
    case 3:
        return "BLOCKED";
    case 4:
        return "EXIT";
    default:
        return 0;
    }
}

