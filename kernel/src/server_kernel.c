#include "server_kernel.h"


void atender_cliente(void *void_args)
{
    t_atender_cliente_args *args = (t_atender_cliente_args *)void_args;

    t_log *logger = args->log;
    int client_socket = args->c_socket;
    char *server_name = args->server_name;

    while (client_socket != -1)
    {   
        log_debug(logger, "Escuchando");
        op_code cop = recibir_operacion(client_socket);

        if (cop == -1)
        {
            t_queue_block* interfaz_desconectada = dictionary_remove(args->planificador->colas.lista_block, client_socket);

            if(interfaz_desconectada == NULL)
            {
                log_error(logger, "Se ha desconectado una interfaz desconocida: %s", interfaz_desconectada->identificador);
            }

            log_debug(logger, "Se deconecta la interfaz: %s", interfaz_desconectada->identificador);
            queue_destroy(interfaz_desconectada->block_queue);
            free(interfaz_desconectada);
            return;
        }

        switch (cop) 
        {
        case PCB:
        {
            t_pcb *pcb_desalojado = recibir_pcb(client_socket);
            bool necesario_planificar = planificador_recepcion_pcb(pcb_desalojado, args->planificador);
            if(necesario_planificar)
            {
                planificador_planificar(args->planificador);
            }
            break;
        }
        case INTERFAZ:
        {   
            t_queue_block *new_client = malloc(sizeof(t_queue_block));
            new_client->identificador = recibir_interfaz(client_socket, logger);
            new_client->socket_interfaz = client_socket;
            new_client->block_queue = queue_create();
            new_client->block_dictionary = list_create();

            dictionary_put(args->planificador->colas.lista_block, new_client->identificador, new_client);
            log_debug(logger, "Se ha conectado la interfaz: %s", new_client->identificador);
            log_debug(logger, "Socket de la conexion: %d", new_client->socket_interfaz);

            break;
        }

        case AVISO_OPERACION_INVALIDA:
        {
            char* nombre_interfaz = recibir_error_oi(client_socket); // TODO: Pedir a Zoe que esto devuelva el nombre de la interfaz
            log_debug(logger, "Se ha recibido una notificacion de operacion invalida");

            t_queue_block *interfaz = dictionary_get(args->planificador->colas.lista_block, nombre_interfaz);
            pcb_a_exit_por_sol_invalida(interfaz, args->planificador);

            break;
        }
        /*case AVISO_OPERACION_VALIDADA:
        {
            logica_int = recibir_op_validada(client_socket);
            log_debug(logger, "La ultima operacion solicitada ha sido validada");
            break;
        }*/
        case AVISO_OPERACION_FINALIZADA:
        {
            char* interfaz_recibida = recibir_op_finalizada(client_socket);
            log_debug(logger, "Operacion finalizada por la interfaz: %s", interfaz_recibida);

            t_queue_block *interfaz = dictionary_get(args->planificador->colas.lista_block, interfaz_recibida);

            procesar_entradasalida_terminada(interfaz, args->planificador);

            break;
        }
        default:
            log_error(logger, "Algo anduvo mal en el server de %s", server_name);
            log_info(logger, "Cop: %d", cop);
            break;
        }
    }

    free(args);
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


