#include <utils/comunicacion.h>

void atender_cliente(void *void_args)
{
    t_atender_cliente_args *args = (t_atender_cliente_args *)void_args;

    t_log *logger = args->log;
    int client_socket = args->c_socket;
    char *server_name = args->server_name;

    
    free(args);

    t_pcb* pcb;
    t_list* listaInstrucciones= abrir_pseudocodigo("prueba_intrucciones.txt");
    
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
        case PC:
        {
            int pc = atoi(recibir_pc(client_socket));

            t_instruccion* instruccion = list_get(listaInstrucciones,pc);
            
            enviar_instruccionSola(client_socket,instruccion);
           
            break;
        }
        /*
        case INTERFAZ:
        {   
            interfaz* new_client = malloc(sizeof(interfaz));
            new_client->nombre_interfaz = recibir_interfaz(client_socket, logger);
            new_client->socket_interfaz = client_socket;
            list_add(interfaces, new_client);
            sem_post(&sem_contador);
            break;
        }*/
        /*case AVISO_DESCONEXION:
        {
            char* interfaz_recibida = recibir_desconexion(client_socket, logger);
            int posicion_interfaz = buscar_interfaz_por_nombre(interfaz_recibida);
            if (posicion_interfaz != -1) {
                interfaz* interfaz_desconectada = (interfaz*)list_remove(interfaces, posicion_interfaz);
                free(interfaz_recibida); 
                if (interfaz_desconectada != NULL) {
                    free(interfaz_desconectada);
                }
            } else {
                log_error(logger, "No se encontró la interfaz %s en la lista", interfaz_recibida);
                free(interfaz_recibida); 
            }
            break;
        }*/
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
/* PROTOCOLO */

/*
int buscar_interfaz_por_nombre(char* nombre_interfaz) {
    int tamanio_lista = list_size(interfaces);
    for (int i = 0; i < tamanio_lista; i++) {
        interfaz* posible_interfaz = list_get(interfaces, i);
        if (string_equals_ignore_case(posible_interfaz->nombre_interfaz, nombre_interfaz)) {
            return i;
        }
    }
    return -1;
}*/


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