#include "server_kernel.h"


void atender_cliente(void *void_args)
{
    t_atender_cliente_args *args = (t_atender_cliente_args *)void_args;

    t_log *logger = args->log;
    int client_socket = args->c_socket;
    char *server_name = args->server_name;

    
    free(args);

    t_pcb* pcb;
    
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
            interfaz* new_client = malloc(sizeof(interfaz));
            new_client->nombre_interfaz = recibir_interfaz(client_socket, logger);
            new_client->socket_interfaz = client_socket;
            new_client->cola_block = queue_create();
            list_add(interfaces, new_client);
            sem_post(&sem_contador);
            break;
        }
        case AVISO_DESCONEXION:
        {
            char* interfaz_recibida = recibir_desconexion(client_socket, logger);
            int posicion_interfaz = buscar_posicion_interfaz_por_nombre(interfaz_recibida);
            if (posicion_interfaz != -1) {
                interfaz* interfaz_desconectada = (interfaz*)list_remove(interfaces, posicion_interfaz);
                free(interfaz_recibida); 
                if (interfaz_desconectada != NULL) {
                    queue_destroy(interfaz_desconectada->cola_block);
                    free(interfaz_desconectada);
                }
            } else {
                log_error(logger, "No se encontró la interfaz %s en la lista", interfaz_recibida);
                free(interfaz_recibida); 
            }
            break;
        }
        case AVISO_OPERACION_INVALIDA:
        {
            logica_int = recibir_error_oi(client_socket);
            break;
        }
        case AVISO_OPERACION_VALIDADA:
        {
            logica_int = recibir_op_validada(client_socket);

            break;
        }
        case AVISO_OPERACION_FINALIZADA:
        {
            //CAMBIAR PCB A ESTADO READY O EXIT
          /*  
            char* interfaz_recibida = recibir_op_finalizada(client_socket, logger);
            int posicion_interfaz = buscar_posicion_interfaz_por_nombre(interfaz_recibida);
            if (posicion_interfaz != -1) {
                interfaz* interfaz_encontrada = (interfaz*)list_get(interfaces, posicion_interfaz);
                //interfaz_encontrada->cola_block = queue_pop(pcb);
                pcb = queue_pop(interfaz_encontrada->cola_block);
                cambiar_a_cola(pcb, READY);
                //DAR SIGNAL DE SEMAFORO PARA MANDAR LA INSTRUCCION
            } else {
                log_error(logger, "No se encontró la interfaz %s en la lista", interfaz_recibida);
                free(interfaz_recibida); 
                */
            }
        default:
            log_error(logger, "Algo anduvo mal en el server de %s", server_name);
            log_info(logger, "Cop: %d", cop);
            break;
        }
            
            
        }
      //log_warning(logger, "El cliente se desconecto de %s server", server_name);  
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


int buscar_posicion_interfaz_por_nombre(char* nombre_interfaz) {
    int tamanio_lista = list_size(interfaces);
    for (int i = 0; i < tamanio_lista; i++) {
        interfaz* posible_interfaz = list_get(interfaces, i);
        if (string_equals_ignore_case(posible_interfaz->nombre_interfaz, nombre_interfaz)) {
            return i;
        }
    }
    return -1;
}