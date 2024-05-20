#include "memoriaserver.h"

t_list* lista_arch;

void atender_cliente(void *void_args)
{
    t_atender_cliente_args *args = (t_atender_cliente_args *)void_args;

    t_log *logger = args->log;
    int client_socket = args->c_socket;
    char *server_name = args->server_name;
    
    free(args);

    t_pcb* pcb;
    

    //MEMORIA a CPU
    
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
            char* path = recibir_mensaje(client_socket, logger);
            lista_arch = abrir_pseudocodigo(path);
            sleep(10);
            free(path);
            
            break;
        }
        case PAQUETE:{}
        case PC:
        {
            char * pc_recibido = recibir_pc(client_socket);
            int pc = atoi(pc_recibido);
            log_info(logger, "Mando la pc %d", pc);
            t_instruccion* instruccion = list_get(lista_arch,pc);
            log_info(logger, "Mando la INSTRUCCION %s", instruccion->buffer->stream);
            enviar_instruccion_mem(client_socket,instruccion);
           
            break;
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