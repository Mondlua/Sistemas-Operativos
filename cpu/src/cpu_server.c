#include "cpu_server.h"

int kernel_socket;

void atender_cliente(void *void_args)
{
    t_atender_cliente_args *args = (t_atender_cliente_args *)void_args;

    t_log *logger = args->log;
    kernel_socket = args->c_socket;
    char *server_name = args->server_name;
    
    free(args);

    t_pcb* pcb;

    //MEMORIA a CPU
    
    while (kernel_socket != -1)
    {   
        op_code cop = recibir_operacion(kernel_socket);

        if (cop == -1)
        {
            log_info(logger, "DISCONNECT!");
            return;
        }

        switch (cop) 
        {
        case MENSAJE:{}
        case PAQUETE:{}
        case PCB:
        {
            pcb = recibir_pcb(kernel_socket);
			log_info(logger, "Lleo a CPU el <PID> es <%u>", pcb->pid);
            char* pc = int_to_char(pcb->p_counter);
            sleep(5);
            realizar_ciclo_inst(conexion_memoria_cpu, pcb);
            enviar_pcb(pcb, kernel_socket);
			break;
        }
            /*  case INSTRUCCION:{
          t_instruccion* ins = recibir_instruccion_cpu(conexion_memoria_cpu);
            log_info(logger, "Me llego la INSTRUCCION %s", ins->buffer->stream);
            t_decode* decodeado= decode(ins);
            log_info(logger, "Me llego el decode %d", decodeado->op_code);
            log_info(logger, "Registro es %s", list_get(decodeado->registroCpu, 0));
           
            break;
        } */
        case FIN_QUANTUM:{
            recibir_interrupcion_finq(kernel_socket);
            hay_interrupcion = 1;
            enviar_motivo(FIN_QUANTUM, kernel_socket);
        }
        
        default:
            log_error(logger, "Algo anduvo mal en el server de %s", server_name);
            log_info(logger, "Cop: %d", cop);
            
            break;
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return;
}

int server_escuchar(void* arg)
{   
    t_atender_cliente_args*args = (t_atender_cliente_args*)arg;
    t_log *logger = args->log;
    char *server_name = args->server_name;
    int server_socket = args->c_socket;
    while (1){
        
        int client_socket = esperar_cliente(server_socket, logger);

        if (client_socket != -1) // != error
        {
            pthread_t hilo;
            args->log = logger;
            args->c_socket = client_socket;
            args->server_name = server_name;
 
            pthread_create(&hilo, NULL, (void *)atender_cliente,  (void*) args); //castear, lo convierto arg a tipo void*
            pthread_detach(hilo); //  hilo se ejecuta de manera independiente, el recurso del hilo se libera automáticamente 
        }
    }
    return 0;
}

void recibir_interrupcion_finq(int kernel_socket){
    int size;
    void* buffer;
    recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
    recv(socket_cliente, buffer, *size, MSG_WAITALL);
    log_info(logger, "Me llego la Interrupcion %d", buffer);
}

void enviar_motivo(op_code motivo, int kernel_socket){

    t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = motivo;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(motivo) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, motivo, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

    int resultado_send = send(kernel_socket, a_enviar, bytes, MSG_NOSIGNAL);

    if (resultado_send == -1) {
        fprintf(stderr, "Error al enviar el mensaje: socket cerrado.\n");
    }
    free(a_enviar);
    eliminar_paquete(paquete);
}