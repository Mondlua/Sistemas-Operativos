#include "memoriaserver.h"

int retardo;

void atender_cliente(void *void_args)
{
    t_atender_cliente_args *args = (t_atender_cliente_args *)void_args;

    t_log *logger = args->log;
    int client_socket = args->c_socket;
    char *server_name = args->server_name;
    uint32_t pid;
    

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
            
            char* mensaje = recibir_pedido(client_socket);
            log_info(logger, "Me llego el pedido de frame\n");

            uint32_t pid;
            int pag;
            sscanf(mensaje, "%u/%d", &pid,&pag);
            
            t_tabla* tabla = buscar_por_pid_return(pid);
            int frame = list_get(tabla->tabla, pag);
            enviar_tamanio_pag_frame(client_socket, frame);
            free(mensaje);
            
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

            uint32_t pid;
            char path [40];
            sscanf(pathpid, "%u/%39[^\n]", &pid,path);          
            t_list* lista_arch = abrir_pseudocodigo(path);
            t_tabla* tabla = malloc(sizeof(t_tabla));
            tabla->pid = pid;
            tabla->tabla = list_create();
            tabla->instrucciones = lista_arch;
            list_add(tabla_pags, tabla);
            log_info(memoria_log, "PID: <%d> - Tamaño: <%d>", pid, list_size(tabla->instrucciones));       
            free(pathpid);

            break;
        }
        case PID:{
            char * pid_recibido =recibir_pc(client_socket);
            pid = atoi(pid_recibido);
            log_info(logger, "Mi PID:%u", pid);
            free(pid_recibido);
            break;
        }
        case PC:
        {
            char * pc_recibido = recibir_pc(client_socket);
            usleep(retardo*1000);
            uint32_t pc = atoi(pc_recibido);

            t_tabla* tabla_pid = list_get(tabla_pags, pid);

            log_debug(logger, "PID obtenido: %d\n", tabla_pid->pid);
            char* instruccion = list_get(tabla_pid->instrucciones, pc);
            eliminar_linea_n(instruccion);
            enviar_mensaje(instruccion, client_socket);
            log_debug(logger, "Mando la instruccion: %s\n", instruccion);

            free(pc_recibido);;
            break;
        }
        case IO_STDIN_READ:{
            instruccion_params* parametros_io = malloc(sizeof(instruccion_params));
            parametros_io = recibir_registro_direccion_tamanio_con_texto(client_socket);
            usleep(retardo*1000);
            //GUARDAR TEXTO EN REGISTRO_DIRECCION
            escribir_en_mem(parametros_io->texto, parametros_io->registro_direccion, parametros_io->registro_tamanio);//VER CAMI EMI 
            enviar_mensaje("OK", client_socket);
            free(parametros_io);
            break;
        }
        case IO_STDOUT_WRITE: {
            instruccion_params* parametros_io = malloc(sizeof(instruccion_params));
            parametros_io = recibir_registro_direccion_tamanio(client_socket);
            usleep(retardo*1000);
            //BUSCAR EN REGISTRO_DIRECCION Y LEER EL REGISTRO_TAMAÑO
            char* mensaje = leer_en_mem(parametros_io->registro_tamanio, parametros_io->registro_direccion); 
            //MANDAR RESULTADO A IO
            enviar_mensaje(mensaje, client_socket);
            free(parametros_io);
            break;

        }
        case IO_FS_READ:{
            instruccion_params* parametros_io = malloc(sizeof(instruccion_params));
            parametros_io = recibir_registro_direccion_tamanio_con_texto(client_socket);
            usleep(retardo*1000);
            //GUARDAR TEXTO EN REGISTRO_DIRECCION
            escribir_en_mem(parametros_io->texto, parametros_io->registro_direccion, parametros_io->registro_tamanio); //VER CAMI EMI
            enviar_mensaje("OK", client_socket);
            free(parametros_io);
            break;
        }
        case IO_FS_WRITE: {
            instruccion_params* parametros_io = malloc(sizeof(instruccion_params));
            parametros_io = recibir_registro_direccion_tamanio(client_socket);
            usleep(retardo*1000);
            //BUSCAR EN REGISTRO_DIRECCION Y LEER EL REGISTRO_TAMAÑO
            char* mensaje = leer_en_mem(parametros_io->registro_tamanio, parametros_io->registro_direccion); //Ver 
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
            int frame = (int)list_get(tabla_pid->tabla, pag);
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
           char* mensaje = recibir_pedido(client_socket);
           log_info(logger, "Me llego el  Pedido de Resize\n");
           usleep(retardo*1000);

           int tamanio;
           uint32_t pid;
           sscanf(mensaje, "%d/%u", &tamanio,&pid);


            t_tabla* tabla_pid = buscar_por_pid_return(pid);
            int cant_pags;
            if(list_size(tabla_pid->tabla) != 0){
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

            if(list_size(tabla_pid->tabla) == 0){
                int cant_pags = tamanio/tam_pagina;
                int cantframes_a_ocupar=  cant_pags;
                size_t count = 0;
                for (size_t i = 0; i < bitarray->size; i++) {
                    if (bitarray_test_bit(bitarray, i) == 0) {
                        count++;
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
                
            }
            }
            log_info(logger, "PID: <%u> - Tamaño Actual: <%d> - Tamaño a Ampliar: <%d>", pid, 0, tamanio); 
            }
           free(mensaje);
           //free(split);
            break;
        }
        case PED_LECTURA:
        {
            char* buffer = recibir_pedido(client_socket);
            log_info(logger, "Me llego el Pedido de Lectura\n"); 
            int tamanio;
            int frame;
            int desp;
            int piid;
            sscanf(buffer, "%d/%d/%d/%u", &tamanio,&frame,&desp,&piid);

            usleep(retardo*1000);


            t_dir_fisica* dir_fisica = malloc(sizeof(t_dir_fisica*));
            dir_fisica->nro_frame = frame;
            dir_fisica->desplazamiento = desp;


            char* leido = leer_en_mem(tamanio, dir_fisica);
            log_info(logger, "PID: %u - Accion:LEER - Direccion fisica: %d - Tamaño %d",piid ,frame+desp,tamanio);

            enviar_mensaje(leido, client_socket);  
            free(buffer);
            free(leido);
            break;
        }
        case PED_ESCRITURA:{
            int tamanio;
            char valor[8];
            int frame; 
            int desp;
            uint32_t pid;
            char* buffer = recibir_pedido(client_socket);
            log_info(logger, "Me llego el Pedido de Escritura \n");  

            sscanf(buffer, "%d/%7[^/]/%d/%d/%d", &tamanio,valor,&frame,&desp,&pid);

            t_dir_fisica* dir_fisica = malloc(sizeof(t_dir_fisica*));
            dir_fisica->nro_frame = frame;
            dir_fisica->desplazamiento = desp;
            usleep(retardo*1000);   
            escribir_en_mem(valor, dir_fisica, tamanio);
            log_info(logger, "PID: %u - Accion:ESCRIBIR - Direccion fisica: %d - Tamaño %d",pid ,frame+desp,tamanio);

            bitarray_set_bit(escrito, frame);

            int frame_siguiente= frame_sig_disp(pid, frame);
         
            enviar_mensaje(int_to_char(frame_siguiente), client_socket);
           
            free(buffer);
           
            break;
        }
        
        case CPY_STRING:{

            int frame1;
            int desp1;
            int frame2;
            int desp2;
            int cantchar;

            char* a_escribir = malloc(sizeof(frame1)+ sizeof(desp1)+sizeof(frame2)+ sizeof(desp2)+ sizeof(cantchar)); 
            a_escribir=recibir_pedido(client_socket);

            log_info(logger, "Me llego el string a escribir: <%s>\n ", a_escribir);

            sscanf(a_escribir, "%d/%d/%d/%d/%d", &frame1,&desp1,&frame2,&desp2,&cantchar);
         
            t_dir_fisica* dir1=malloc(sizeof(t_dir_fisica*)) ;//direc
            t_dir_fisica* dir2= malloc(sizeof(t_dir_fisica*)); //stri
            dir1->nro_frame = frame1;
            dir1->desplazamiento=desp1;
            dir2->nro_frame = frame2;
            dir2->desplazamiento=desp2;

            usleep(retardo*1000);
            
            char* leido = leer_en_mem(sizeof(char*), dir2);
            char* cortado = string_substring_until(leido, cantchar);
            escribir_en_mem(cortado, dir1,sizeof(char*));

            free(leido);
            free(a_escribir);
            free(cortado);
            free(dir1);
            free(dir2);
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
            free(pidc);

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
    free(args);

    return 0;
}



void eliminar_linea_n(char* linea){
    if(linea[strlen(linea)-1] == '\n'){
        linea[strlen(linea)-1]='\0';
    }
}

