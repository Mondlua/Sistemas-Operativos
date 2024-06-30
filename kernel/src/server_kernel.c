#include "server_kernel.h"


void atender_cliente(void *void_args)
{
    t_atender_cliente_args *args = (t_atender_cliente_args *)void_args;

    t_log *logger = args->log;
    int client_socket = args->c_socket;
    char *server_name = args->server_name;

    
    free(args);
    
    while (client_socket != -1)
    {   
        op_code cop = recibir_operacion(client_socket);

        if (cop == -1)
        {
            interfaz* posible_interfaz = buscar_posicion_interfaz_por_cliente(client_socket);
            if (posible_interfaz != NULL) {
                if (list_remove_element(interfaces, posible_interfaz)) {
                    queue_destroy(posible_interfaz->cola_block);
                    sem_destroy(&posible_interfaz->semaforo_interfaz);
                    log_info(logger, "DISCONNECT %s!", posible_interfaz->nombre_interfaz);
                    free(posible_interfaz->nombre_interfaz);
                    free(posible_interfaz);
                }
            } else {
                log_error(logger, "No se encontró la interfaz %s en la lista", posible_interfaz->nombre_interfaz);
            }
            return;
        }

        switch (cop) 
        {
        case PCB:
        {
            t_pcb *pcb_desalojado = recibir_pcb(client_socket);
            // bool necesario_planificar = planificador_recepcion_pcb(pcb_desalojado);
            // if(necesario_planificar)
            // {
            //     planificador_planificar();
            // }
            break;
        }
        case INTERFAZ:
        {   
            interfaz* new_client = malloc(sizeof(interfaz));
            new_client->nombre_interfaz = recibir_interfaz(client_socket, logger);
            new_client->socket_interfaz = client_socket;
            sem_init(&new_client->semaforo_interfaz, 0, 1);
            new_client->cola_block = queue_create();
            list_add(interfaces, new_client);
            sem_post(&sem_contador_int);
            break;
        }
        case AVISO_OPERACION_INVALIDA:
        {
            logica_int = recibir_error_oi(client_socket);
            sem_post(&habilitacion_io);
            break;
        }
        case AVISO_OPERACION_VALIDADA:
        {
            logica_int = recibir_op_validada(client_socket);
            sem_post(&habilitacion_io);
            break;
        }
        case AVISO_OPERACION_FINALIZADA:
        {
            char* interfaz_recibida = recibir_op_finalizada(client_socket);
            int posicion_interfaz = buscar_posicion_interfaz_por_nombre(interfaz_recibida);
            if (posicion_interfaz != -1) {
                interfaz* interfaz_encontrada = (interfaz*)list_get(interfaces, posicion_interfaz);
                t_pcb* pcb=(t_pcb*)queue_pop(interfaz_encontrada->cola_block);
                cambiar_a_cola(pcb, READY);
                sem_post(&interfaz_encontrada->semaforo_interfaz);
            } else {
                log_error(logger, "No se encontró la interfaz %s en la lista", interfaz_recibida);
                free(interfaz_recibida); 
            }
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

interfaz* buscar_posicion_interfaz_por_cliente(int cliente) {
    int tamanio_lista = list_size(interfaces);
    for (int i = 0; i < tamanio_lista; i++) {
        interfaz* posible_interfaz = list_get(interfaces, i);
        if (posible_interfaz->socket_interfaz == cliente) {
            return posible_interfaz;
        }
    }
    return NULL;
}