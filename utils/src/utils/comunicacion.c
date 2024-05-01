#include <utils/comunicacion.h>

t_list* interfaces;

void atender_cliente(void *void_args)
{
    t_atender_cliente_args *args = (t_atender_cliente_args *)void_args;

    t_log *logger = args->log;
    int client_socket = args->c_socket;
    char *server_name = args->server_name;

    
    free(args);

    op_code cop = recibir_operacion(client_socket);
    t_pcb* pcb;
    cliente new_client;
    while (client_socket != -1)
    {   
        op_code cop = recibir_operacion(client_socket);

        if (cop == -1)
        {
            log_info(logger, "DISCONNECT!");
            return;
        }

        switch (cop) 
        {
        case MENSAJE:
        {
            recibir_mensaje(client_socket, logger);

            break;
        }
        case PAQUETE:
        {/*
            pcb = recibir_pcb(client_socket);
			log_info(logger, "Me llego el pcb cuyo pid es %u", pcb->pid);
			break;*/
        }

        case INTERFAZ:
        {   
            new_client.nombre_cliente = strdup(recibir_interfaz(client_socket, logger));
            new_client.socket_cliente = client_socket;
            list_add(interfaces, &new_client);
            free(new_client.nombre_cliente);

            break;
        }
        case AVISO_DESCONEXION:
        {
            char* interfaz = recibir_desconexion(client_socket, logger);
            list_remove_element(interfaces, interfaz);
            free(interfaz);
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
            log_info(logger, "cree hilo");

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
/* PROTOCOLO */



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

/*t_pcb* recibir_pcb(int socket_cliente) {
    t_list* valores_paquete = recibir_paquete(socket_cliente);
    if (valores_paquete == NULL) {
        return NULL;
    }

    t_pcb* pcb = malloc(sizeof(t_pcb));
    

    pcb->pid = *((uint32_t *) list_get(valores_paquete, 0)) ;
    pcb->p_counter = *((int*) list_get(valores_paquete, 1));
    pcb -> quantum = *((int*) list_get(valores_paquete, 2));
    pcb -> estado = *((t_proceso_estado*) list_get(valores_paquete, 3));

    list_destroy(valores_paquete);
    return pcb;
}
*/