#include <utils/comunicacion.h>

typedef struct
{
    t_log *log;
    int c_socket;
    char *server_name;

} t_atender_cliente_args;

void atender_cliente(void *void_args)
{
    t_atender_cliente_args *args = (t_atender_cliente_args *)void_args;
    t_log *logger = args->log;
    int client_socket = args->c_socket;
    char *server_name = args->server_name;
    free(args);

    op_code cop = recibir_operacion(client_socket);
    while (client_socket != -1)
    {

        if (cop == -1)
        {
            log_info(logger, "DISCONNECT!");
            return;
        }

        switch (cop)
        {
     //   case HANDSHAKE:
   //     {
      //      handshake_kernel()
     //   }
        
        case MENSAJE:
        {
            recibir_mensaje(client_socket, logger);

            break;
        }

        case CREAR_PROCESO:
        {
            break;
        }

        case FINALIZAR_PROCESO:
        {
            break;
        }

        case ACCEDER_TABLAS_P:
        {
            break;
        }

        case AJUSTAR_TAMANIO:
        {
            break;
        }

        case ACCEDER_ESPACIO_U:
        {
            break;
        }

        // Errores
      /*  case -1:   [mas arriba esta el caso de desconeccion]
            log_error(logger, "Cliente desconectado de %s...", server_name);
            return;*/
        default:
            log_error(logger, "Algo anduvo mal en el server de %s", server_name);
            log_info(logger, "Cop: %d", cop);
            return;
            
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return;
}

int server_escuchar(t_log *logger, char *server_name, int server_socket)
{
    while (1){
        
        int client_socket = esperar_cliente(server_socket, logger);

        if (client_socket != -1) // != error
        {
            log_info(logger, "cree hilo");

            pthread_t hilo;
            t_atender_cliente_args *args = malloc(sizeof(t_atender_cliente_args));
            args->log = logger;
            args->c_socket = client_socket;
            args->server_name = server_name;
 
            pthread_create(&hilo, NULL, (void *)atender_cliente,  (void*) args); //castear, lo convierto arg a tipo void*
            pthread_detach(hilo); //  hilo se ejecuta de manera independiente, el recurso del hilo se liberará automáticamente 
        }
    }
    return 0;
}
