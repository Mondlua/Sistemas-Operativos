#include "memoriaserver.h"

t_list* lista_arch;
int retardo;

void atender_cliente(void *void_args)
{
    t_atender_cliente_args *args = (t_atender_cliente_args *)void_args;

    t_log *logger = args->log;
    int client_socket = args->c_socket;
    char *server_name = args->server_name;
    uint32_t pid;
    free(args);

    retardo = config_get_int_value(memoria_config, "RETARDO_RESPUESTA");

    //MEMORIA a CPU
    
    while (client_socket != -1)
    {   
        op_code cop = recibir_operacion(client_socket);
        // usleep(retardo*1000);

        if (cop == -1)
        {
            log_info(logger, "DISCONNECT!");
            return;
        }

        switch (cop) 
        {
        case FRAME:{
            
            char* pidpag = recibir_pedido_frame(client_socket, logger);
            char** split = string_split(pidpag, "/");
            uint32_t pid = atoi(split[0]);
            int pag = atoi(split[1]);
            t_tabla* tabla = buscar_por_pid_return(pid);
            int frame = list_get(tabla->tabla, pag);
            enviar_tamanio_pag_frame(client_socket, frame);
            break;
        }
        case TAM_PAG: {
            recibir_ped_tamanio_pag(client_socket, logger);
            enviar_tamanio_pag_frame(client_socket, tam_pagina);
           break;  
        }
        case MENSAJE:
        {
            char* pathpid = recibir_mensaje(client_socket, logger);

            char** split = string_split(pathpid, "$");
            char* path = split[0];
            uint32_t pid = atoi(split[1]);

            lista_arch = list_create();
            lista_arch = abrir_pseudocodigo(path);
            free(path);

            t_tabla* tabla = malloc(sizeof(t_tabla));
            tabla->pid = pid;
            tabla->tabla = list_create();
            list_add(tabla_pags, tabla);
            log_info(memoria_log, "PID: <%d> - Tamaño: <%d>", pid, list_size(tabla->tabla));

            break;
        }
        case PID:{
            char * pid_recibido =recibir_pc(client_socket);
            pid = atoi(pid_recibido);
            log_info(logger, "Mi PID:%u", pid);
            break;
        }
        case PC:
        {
            char * pc_recibido = recibir_pc(client_socket);
            //usleep(retardo*1000);
            uint32_t pc = atoi(pc_recibido);
            sem_wait(&semaforo_mem);
            t_instruccion* instruccion = (t_instruccion*)list_get(lista_arch,pc);
            char* ins  = instruccion->buffer->stream;
            eliminar_linea_n(ins);
            log_debug(logger, "Mando la instruccion: %s", instruccion->buffer->stream);
            enviar_instruccion_mem(client_socket,instruccion);
           
            break;
        }
        case IO_STDIN_READ:{
            instruccion_params* parametros_io = malloc(sizeof(instruccion_params));
            parametros_io = recibir_io_stdin(client_socket);
            usleep(retardo*1000);
            //GUARDAR TEXTO EN REGISTRO_DIRECCION
            escribir_en_mem(parametros_io->texto, parametros_io->params.io_stdin_stdout.registro_direccion);
            enviar_mensaje("OK", client_socket);
            free(parametros_io);
            break;
        }
        case IO_STDOUT_WRITE: {
            instruccion_params* parametros_io = malloc(sizeof(instruccion_params));
            parametros_io = recibir_io_stdout(client_socket);
            usleep(retardo*1000);
            //BUSCAR EN REGISTRO_DIRECCION Y LEER EL REGISTRO_TAMAÑO
            char* mensaje = leer_en_mem(parametros_io->params.io_stdin_stdout.registro_tamaño, parametros_io->params.io_stdin_stdout.registro_direccion);
            //MANDAR RESULTADO A IO
            enviar_mensaje(mensaje, client_socket);
            free(parametros_io);
            break;

        }
        case ACCESO_TABLA:
        {
            char* pidpag = recibir_mensaje(client_socket, logger);
            usleep(retardo*1000);
            char** split = string_split(pidpag, "$");
            uint32_t pid = split[0];
            int pag = atoi(split[1]);
            t_tabla* tabla_pid = buscar_por_pid_return(pid);
            if(pag<list_size(tabla_pid->tabla)){
            int frame = list_get(tabla_pid->tabla, pag);
            enviar_mensaje(int_to_char(frame), client_socket); //ver funcion de enviar(crearla)
            log_info(logger,"PID: %u - Pagina: %d - Marco: %d",pid, pag, frame);
            }
            else{
                log_error(logger, "No se pudo acceder a la pagina <%d> de la Tabla del Pid <%u>",pag, pid);
            }
            break;
        }
        case CPU_RESIZE:
        {   
           char* mensaje = recibir_pedido_resize_tampid(client_socket, logger);
           usleep(retardo*1000);
           char** split = string_split(mensaje, "/");
           uint32_t pid= atoi(split[0]);
           int tamanio = atoi(split[1]);

            t_tabla* tabla_pid = buscar_por_pid_return(pid);
            int cant_pags;
            if(tabla_pid != NULL){
            cant_pags = list_size(tabla_pid->tabla);
            int tamanio_pid = cant_pags * tam_pagina;
            if(tamanio > tamanio_pid){
            //AMPLIAR PROCESO
                int bytes_a_ampliar = tamanio - tamanio_pid;
                int cantframes_a_ocupar=  bytes_a_ampliar/tam_pagina;
                size_t count = 0;
                for (size_t i = 0; i < bitarray->size; i++) {
                    if (bitarray_test_bit(bitarray, i) == 0) {
                        count++;
                    }
                }
                if(count>=cantframes_a_ocupar){
                    int frames_ocupados=0;
                    for (int i = 0; i < bitarray->size; i++) {
                         if (bitarray_test_bit(bitarray, i) == 0) {
                            bitarray_set_bit(bitarray, i);
                            list_add(tabla_pid->tabla, i);
                            frames_ocupados++;
                        }
                         if (frames_ocupados == cantframes_a_ocupar) {
                            break;
                       }
                    }
                log_info(logger, "PID: <%u> - Tamaño Actual: <%d> - Tamaño a Ampliar: <%d>", pid, tamanio_pid, tamanio); 
                }
                else{
                    log_error(logger, "Out Of Memory");
                    // mandarle mensaje al cpu en el case de resize para que envie el contexto al kernel
                    //INTERRUPCION OUT OF MEMORY
                } 
            }     
            else{
                //REDUCIR PROCESO
               
                int bytes_a_reducir = tamanio - tamanio_pid;
                int cantframes_a_reducir=  bytes_a_reducir/tam_pagina;
                int cant_pags_nueva = cant_pags - cantframes_a_reducir;
                                
                for(int i=(cant_pags-1); i>cant_pags_nueva; i--){
                    int frame = list_get(tabla_pid->tabla, i);
                    list_remove(tabla_pid->tabla, i);
                    bitarray_clean_bit(bitarray,frame);
                }
                log_info(logger,"PID: <%d> - Tamaño Actual: <%d> - Tamaño a Reducir: <%d>", pid,tamanio_pid, tamanio);
            }
            }
           
            break;
        }
        case PED_LECTURA:
        {
            t_list* buffer = recibir_pedido_lectura(client_socket, logger); 
            usleep(retardo*1000);
            t_dir_fisica* dir_fisica = list_get(buffer,0);
            // RECIBIR TAMANIO A LEER
            int tamanio = list_get(buffer,1);
            char* leido = leer_en_mem(tamanio, dir_fisica);
            enviar_mensaje(leido, client_socket);
            free(leido);   
            break;
        }
        case PED_ESCRITURA:{
            t_dir_fisica* dir_fisica = recibir_pedido_escritura(client_socket, logger); 
            usleep(retardo*1000);
            uint8_t valor = recibir_valor_escritura(client_socket, logger);
            escribir_en_mem(int_to_char(valor), dir_fisica);

            break;
        }
        
        case CPY_STRING:{
            char* a_escribir = recibir_cpy_string(client_socket, logger);
            usleep(retardo*1000);
            //escribir_a_mem(a_escribir, void* donde);
            break;
        }
        case FINALIZACION:
        {
            char* pidc = recibir_mensaje(client_socket, logger);
            usleep(retardo*1000);
            uint32_t pid = atoi(pidc);

            t_tabla* tabla_pid = list_remove(tabla_pags, buscar_por_pid_return(pid));
            for(int i=0; i<list_size(tabla_pid->tabla); i++){
                int frame = list_get(tabla_pid->tabla, i);
                bitarray_clean_bit(bitarray, frame);
            }

            log_info(logger, "PID: <%u> - Tamaño: <%d>", pid, list_size(tabla_pid->tabla));

            free(tabla_pid);

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



void eliminar_linea_n(char* linea){
    if(linea[strlen(linea)-1] == '\n'){
        linea[strlen(linea)-1]='\0';
    }
}

