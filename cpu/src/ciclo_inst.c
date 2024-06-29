#include "ciclo_inst.h"

t_instruccion* fetch(int conexion, t_pcb* pcb){
    
    t_instruccion* instruccion;
    enviar_pid(int_to_char(pcb->pid), conexion);
    enviar_pc(int_to_char(pcb->registros->PC),conexion);
    instruccion = recibir_instruccion_cpu(conexion);
    
    pcb->registros->PC++;

    return instruccion;
}

void eliminar_linea_n(char* linea){
    if(linea[strlen(linea)-1] == '\n'){
        linea[strlen(linea)-1]='\0';
    }
}

instrucciones obtener_instruccion(char *nombre) {
    if (strcmp(nombre, "SET") == 0) 
    {
        return SET;
    }
    if (strcmp(nombre, "MOV_IN") == 0) 
    {
        return MOV_IN;
    }
    if (strcmp(nombre, "MOV_OUT") == 0) 
    {
        return MOV_OUT;
    } 
    if (strcmp(nombre, "SUM") == 0) {
        return SUM;
    } 
    if (strcmp(nombre, "SUB") == 0) {
        return SUB;
    } 
    if (strcmp(nombre, "JNZ") == 0) {
        return JNZ;
    } 
    if (strcmp(nombre, "RESIZE") == 0) {
        return RESIZE;
    } 
    if (strcmp(nombre, "COPY_STRING") == 0) {
        return COPY_STRING;
    } 
    if (strcmp(nombre, "WAIT") == 0) {
        return WAIT;
    } 
    if (strcmp(nombre, "SIGNAL") == 0) {
        return SIGNAL;
    } 
    if (strcmp(nombre, "IO_GEN_SLEEP") == 0) {
        return IO_GEN_SLEEP;
    } 
    if (strcmp(nombre, "IO_STDIN_READ") == 0) {
        return IO_STDIN_READ;
    } 
    if (strcmp(nombre, "IO_STDOUT_WRITE") == 0) {
        return IO_STDOUT_WRITE;
    } 
    if (strcmp(nombre, "IO_FS_CREATE") == 0) {
        return IO_FS_CREATE;
    } 
    if (strcmp(nombre, "IO_FS_DELETE") == 0) {
        return IO_FS_DELETE;
    } 
    if (strcmp(nombre, "IO_FS_TRUNCATE") == 0) {
        return IO_FS_TRUNCATE;
    } 
    if (strcmp(nombre, "IO_FS_WRITE") == 0) {
        return IO_FS_WRITE;
    } 
    if (strcmp(nombre, "IO_FS_READ") == 0) {
        return IO_FS_READ;
    } 
    if (strcmp(nombre, "EXIT") == 0) {
        return EXIIT;
    } 
    
    return EXIIT;
}

t_decode* decode(t_instruccion* instruccion){

    char* buffer = (char*) instruccion->buffer->stream;
    eliminar_linea_n(buffer);
    char** arrayIns = string_split(buffer," ");
    printf("Instruccion: %s.\n", arrayIns[0]);

    instrucciones ins = obtener_instruccion(arrayIns[0]);
    t_decode* decode = malloc(sizeof(t_decode));
    decode->op_code = ins;
    decode->registroCpu = list_create();
    
    switch(ins){
        case SET:{
        char* registro = strdup(arrayIns[1]);
        list_add(decode->registroCpu, registro);
        int valor =atoi(strdup(arrayIns[2]));
        decode->valor = valor;     
        break;     
        }
        case MOV_IN:{
        char* registroDatos = strdup(arrayIns[1]);
        list_add(decode->registroCpu,registroDatos);
        char* registroDireccion = strdup(arrayIns[2]);
        list_add(decode->registroCpu,registroDireccion);     
        break;
        }
        case MOV_OUT:{
        char* registroDireccion = strdup(arrayIns[1]);
        list_add(decode->registroCpu,registroDireccion);
        char* registroDatos = strdup(arrayIns[2]);
        list_add(decode->registroCpu,registroDatos);   
        decode->logicaAFisica= true;  
        break;
        }
        case SUM:{
        char* registroDestino = strdup(arrayIns[1]);
        list_add(decode->registroCpu,registroDestino);
        char* registroOrigen= strdup(arrayIns[2]);
        list_add(decode->registroCpu,registroOrigen);     
        break;
        }
        case SUB:{
        char* registroDestino = strdup(arrayIns[1]);
        list_add(decode->registroCpu,registroDestino);
        char* registroOrigen= strdup(arrayIns[2]);
        list_add(decode->registroCpu,registroOrigen);
        break;
        }
        case JNZ:{
        char* registro=strdup(arrayIns[1]);
        list_add(decode->registroCpu,registro);
        instrucciones ins = atoi(strdup(arrayIns[2]));
        decode->instrucciones= ins;
        break;
        }
        case RESIZE:{
        int tamanio = atoi(strdup(arrayIns[1]));
        decode->valor= tamanio;
        break;
        }   
        case COPY_STRING:{
        int tamanio = atoi(strdup(arrayIns[1]));
        decode->valor= tamanio;
        break;
        }
        case WAIT:{
        char *rec;
        rec= strdup(arrayIns[1]);
        decode->recurso = rec;
        break;
        }
        case SIGNAL:{
        char* recu = strdup(arrayIns[1]);
        decode->recurso=recu;
        break;
        }
        case IO_GEN_SLEEP:{
        char* interfaz = strdup(arrayIns[1]);
        decode->interfaz = interfaz;
        int unidadesTrabajo = atoi(strdup(arrayIns[2]));
        decode->valor = unidadesTrabajo;
        break;
        }
        case IO_STDIN_READ:{
        char* interfaz = strdup(arrayIns[1]);
        decode->interfaz = interfaz;
        char* registro_direccion = arrayIns[2];
        list_add(decode->registroCpu, registro_direccion);
        char* registro_tamaño = arrayIns[3];
        list_add(decode->registroCpu, registro_tamaño);  
        break;
        }
        case IO_STDOUT_WRITE:{
        char* interfaz = strdup(arrayIns[1]);
        decode->interfaz = interfaz;
        char* registro_direccion = arrayIns[2];
        list_add(decode->registroCpu, registro_direccion);
        char* registro_tamaño = arrayIns[3];
        list_add(decode->registroCpu, registro_tamaño);  
        break;
        }
        case IO_FS_CREATE:{
            break;
        }
        case IO_FS_DELETE:{
            break;
        }
        case IO_FS_TRUNCATE:{
            break;
        }
        case IO_FS_WRITE:{
            break;
        }
        case IO_FS_READ:{
            break;
        }
        case EXIIT:{
            break;
        }

    }
    for (int i = 0; arrayIns[i] != NULL; i++) {
    free(arrayIns[i]);
    }
    free(arrayIns);
    return decode;
}

void asignar_registro(cpu_registros* regs, const char* nombre_registro, void* valor) {
    if (strcmp(nombre_registro, "PC") == 0)  {regs->PC=valor;}
    else if (strcmp(nombre_registro, "AX") == 0)  {regs->AX=valor;}
    else if (strcmp(nombre_registro, "BX") == 0)  {regs->BX=valor;}
    else if (strcmp(nombre_registro, "CX") == 0)  {regs->CX=valor;}
    else if (strcmp(nombre_registro, "DX") == 0)  {regs->DX=valor;}
    else if (strcmp(nombre_registro, "EAX") == 0)  {regs->EAX=valor;}
    else if (strcmp(nombre_registro, "EBX") == 0)  {regs->EBX=valor;}
    else if (strcmp(nombre_registro, "ECX") == 0) {regs->ECX=valor;}
    else if (strcmp(nombre_registro, "EDX") == 0)  {regs->EDX=valor;}
    else if (strcmp(nombre_registro, "SI") == 0)  {regs->SI=valor;}
    else if (strcmp(nombre_registro, "DI") == 0)  {regs->DI=valor;}
}

void* obtener_valor_registro(cpu_registros* regs, char* nombre_registro) {
    if (strcmp(nombre_registro, "PC") == 0) {return regs->PC;}
    else if (strcmp(nombre_registro, "AX") == 0) {return regs->AX;}
    else if (strcmp(nombre_registro, "BX") == 0) {return regs->BX;}
    else if (strcmp(nombre_registro, "CX") == 0) {return  regs->CX;}
    else if (strcmp(nombre_registro, "DX") == 0) {return regs->DX;}
    else if (strcmp(nombre_registro, "EAX") == 0) {return regs->EAX;}
    else if (strcmp(nombre_registro, "EBX") == 0) {return regs->EBX;}
    else if (strcmp(nombre_registro, "ECX") == 0) {return regs->ECX;}
    else if (strcmp(nombre_registro, "EDX") == 0) {return regs->EDX;}
    else if (strcmp(nombre_registro, "SI") == 0) {return regs->SI;}
    else if (strcmp(nombre_registro, "DI") == 0) {return regs->DI;}
    else {return NULL;}
}

int obtener_tipo_registro(char* nombre_registro){
    if (strcmp(nombre_registro, "PC") == 0 || strcmp(nombre_registro, "AX") == 0 || 
        strcmp(nombre_registro, "BX") == 0 || strcmp(nombre_registro, "CX") == 0 || 
        strcmp(nombre_registro, "DX") == 0) {
        return 8;
    } else {
        return 32;
    }
}

t_cpu_blockeo execute(t_decode* decode, t_pcb* pcb, t_log *logger){
    
    instrucciones ins = decode->op_code;
    switch(ins){
    
        case SET:{
            uint8_t valor = (uint8_t) decode->valor;
            char* registro_adepositar = list_get(decode->registroCpu,0);
            log_info(logger, "PID: %d - Ejecutando: SET %s %d", pcb->pid, registro_adepositar, valor);        
            asignar_registro(pcb->registros, registro_adepositar, valor);    
            break;
         }
        case MOV_IN:{
            char* registro_datos = list_get(decode->registroCpu,0);
            char* registro_direccion = list_get(decode->registroCpu,1);
            uint8_t dir_logica = (uint8_t) obtener_valor_registro(pcb->registros, registro_direccion);
            t_dir_fisica* dir_fisica = mmu(dir_logica, pcb->pid);

            int tamanio;

            if(obtener_tipo_registro(registro_datos) == 8){
                tamanio =1;
            }
            else{
                tamanio = 4;
            }

            int num_frame = dir_fisica->nro_frame;
            int desplazamiento = dir_fisica->desplazamiento;

            char* tam = strcat(int_to_char(tamanio), "/");
            char* tamframe = strcat(tam, int_to_char(num_frame));
            char* tamframe1 = strcat(tamframe, "/");
            char* enviar = strcat(tamframe1, int_to_char(desplazamiento));
            enviar_pedido_lectura(conexion_memoria_cpu, enviar);
            int i = recibir_operacion(conexion_memoria_cpu);
            char* leido = recibir_mensaje(conexion_memoria_cpu, cpu_log);
            asignar_registro(pcb->registros, registro_datos, atoi(leido));
            free(leido);
            break;
        }
        case MOV_OUT:{

            char* registro_datos = list_get(decode->registroCpu,1);
            char* registro_direccion = list_get(decode->registroCpu,0);
            uint8_t dir_logica = (uint8_t) obtener_valor_registro(pcb->registros, registro_direccion);

            uint8_t valor = (uint8_t) obtener_valor_registro(pcb->registros, registro_datos);
            int tamanio;

            if(obtener_tipo_registro(registro_datos) == 8){
                tamanio =1;
            }
            else{
                tamanio = 4;
            }

            char* aescribir = int_to_char(valor);

            int cant_pags = tamanio/tam_pag;

            if(cant_pags <=1){        
                t_dir_fisica* dir_fisica = mmu(dir_logica, pcb->pid);

                int num_frame = dir_fisica->nro_frame;
                int desplazamiento = dir_fisica->desplazamiento;
                
                printf ("num frame %d\n", num_frame);
                char* valorr= strcat(int_to_char(tamanio), "/");
                char* valorrr= strcat(valorr, aescribir);
                char* valorrrr= strcat(valorrr, "/");
                char* tamframe = strcat(valorrrr, int_to_char(num_frame));
                char* tamframe1 = strcat(tamframe, "/");
                char* enviar1 = strcat(tamframe1, int_to_char(desplazamiento));
                char* enviar2 = strcat(enviar1, "/");
                char* enviar = strcat(enviar2, int_to_char(pcb->pid));

                enviar_pedido_escritura(conexion_memoria_cpu, enviar);
                int i = recibir_operacion(conexion_memoria_cpu);
                char* frame_siguiente = recibir_mensaje(conexion_memoria_cpu,cpu_log);
                printf("Entre al if de move out\n y el valor es %s, en num %d, y frame sig %s", int_to_char(valor), atoi(int_to_char(valor)), frame_siguiente);
            }/*
            else{
                printf("Entre al move out con mas de una pagina\n");

                int cant_partes; 
                char** parts = split_por_bytes(aescribir, tam_pag, &cant_partes); 

                t_dir_fisica* dir_fisica = mmu(dir_logica, pcb->pid);

                for (int i = 0; i < cant_partes; i++) {
                    printf("Parte %d: %s\n en %d", i + 1, parts[i], int_to_char(parts[i]));

                    int num_frame = dir_fisica->nro_frame;
                    int desplazamiento = dir_fisica->desplazamiento;
                    
                    char* valorr= strcat(parts[i], "/");
                    char* tamframe = strcat(valorr, int_to_char(num_frame));
                    char* tamframe1 = strcat(tamframe, "/");
                    char* enviar1 = strcat(tamframe1, int_to_char(desplazamiento));
                    char* enviar2 = strcat(enviar1, "/");
                    char* enviar = strcat(enviar2, int_to_char(pcb->pid));

                    enviar_pedido_escritura(conexion_memoria_cpu, enviar);
                    int i = recibir_operacion(conexion_memoria_cpu);
                    char* frame_siguiente = recibir_mensaje(conexion_memoria_cpu,cpu_log);
                    
                    dir_fisica->nro_frame = frame_siguiente;
                    dir_fisica->desplazamiento = 0;

                    free(parts[i]); 
                }

                free(parts); 
            }*/
            break; 
        }
        case SUM:{
            char* registroDestino = (char*)list_get(decode->registroCpu,0);
            char* registroOrigen = (char*)list_get(decode->registroCpu,1);
            uint8_t valor2 = (uint8_t) obtener_valor_registro(pcb->registros, registroDestino);
            uint8_t valor1 = (uint8_t) obtener_valor_registro(pcb->registros, registroOrigen);
            uint8_t suma = valor1 + valor2;
            asignar_registro(pcb->registros, registroDestino, suma);
            break;
        }
        case 4:{
            char* registroDestino=(char*)list_get(decode->registroCpu, 0);
            char* registroOrigen=(char*)list_get(decode->registroCpu, 1);
            uint8_t valor2 = (uint8_t) obtener_valor_registro(pcb->registros, registroDestino);
            uint8_t valor1 = (uint8_t) obtener_valor_registro(pcb->registros, registroOrigen);
            uint8_t resta = valor2 - valor1;
            asignar_registro(pcb->registros, registroDestino, resta);
            break;
        }
        case 5:{
            char* registro = (char*)list_get(decode->registroCpu, 0);
            uint8_t valor=obtener_valor_registro(pcb->registros, registro);
            if(valor!=0){
                instrucciones ins= decode->instrucciones;
                //pcb->p_counter=ins;
            }
            break;
        }
        //resize
        case 6:{
            int tamanio_resize = decode->valor;
            char* tamanio = int_to_char(tamanio_resize);
            char* pid_char = int_to_char(pcb->pid);
            char* mensaje1 = strcat(pid_char,"/");
            char* mensaje = strcat(mensaje1,tamanio);
            enviar_pedido_resize_tampid(conexion_memoria_cpu, mensaje);
            break;
        }
        case 7:{
            int bytes = decode->valor;
            int cant_char = bytes / sizeof(char*);
            char* string =obtener_valor_registro(pcb->registros, "SI");
            void* dir_apuntada =obtener_valor_registro(pcb->registros, "DI");
        
        	char* string_cortado= string_substring_until(string,cant_char);
            enviar_cpy_string(conexion_memoria_cpu, string_cortado);
            break;
        }
        case 8:{}
        case 9:{}
        case 10:{
            enviar_motivo(BLOCK_IO, kernel_socket);
            instruccion_params* parametros =  malloc(sizeof(instruccion_params));
            parametros->interfaz = strdup(decode->interfaz);
            parametros->params.io_gen_sleep_params.unidades_trabajo = decode->valor;

            t_paquete_instruccion* paquete = malloc(sizeof(t_paquete_instruccion));
            paquete->codigo_operacion = IO_GEN_SLEEP;
            enviar_instruccion_a_Kernel(paquete, parametros, kernel_socket);
            free(parametros->interfaz); 
            free(parametros);
            free(paquete);
            break;
        }
        case 11:{
            enviar_motivo(BLOCK_IO, kernel_socket);
            instruccion_params* parametros =  malloc(sizeof(instruccion_params));
            parametros->interfaz = strdup(decode->interfaz);
            char* registro_direccion = (char*)list_get(decode->registroCpu, 0);
            char* registro_tamaño = (char*)list_get(decode->registroCpu, 1);
            int dir_logica =(int)obtener_valor_registro(pcb->registros, registro_direccion);
            t_dir_fisica* dir_fisica = mmu(dir_logica, pcb->pid);
            parametros->params.io_stdin_stdout.registro_direccion = dir_fisica;
            parametros->params.io_stdin_stdout.registro_tamaño = (cpu_registros*)obtener_valor_registro(pcb->registros, registro_tamaño);
            t_paquete_instruccion* paquete = malloc(sizeof(t_paquete_instruccion));
            paquete->codigo_operacion = IO_STDIN_READ;
            enviar_instruccion_a_Kernel(paquete, parametros, kernel_socket);
            free(parametros->interfaz); 
            free(parametros);
            free(paquete);
            break;
        }
        case 12:{
            enviar_motivo(BLOCK_IO, kernel_socket);
            instruccion_params* parametros =  malloc(sizeof(instruccion_params));
            parametros->interfaz = strdup(decode->interfaz);
            char* registro_direccion = (char*)list_get(decode->registroCpu, 0);
            char* registro_tamaño = (char*)list_get(decode->registroCpu, 1);
            int dir_logica =(int)obtener_valor_registro(pcb->registros, registro_direccion);
            t_dir_fisica* dir_fisica = mmu(dir_logica, pcb->pid);
            parametros->params.io_stdin_stdout.registro_direccion = dir_fisica;
            parametros->params.io_stdin_stdout.registro_direccion = (cpu_registros*)obtener_valor_registro(pcb->registros, registro_direccion);
            parametros->params.io_stdin_stdout.registro_tamaño = (cpu_registros*)obtener_valor_registro(pcb->registros, registro_tamaño);
            t_paquete_instruccion* paquete = malloc(sizeof(t_paquete_instruccion));
            paquete->codigo_operacion = IO_STDOUT_WRITE;
            enviar_instruccion_a_Kernel(paquete, parametros, kernel_socket);
            free(parametros->interfaz); 
            free(parametros);
            free(paquete);
            break;
        }
        case 13:{}
        case 14:{}
        case 15:{}
        case 16:{}
        case 17:{}
        case EXIIT:{
            log_info(logger, "PID: %d - Ejecutando: EXIT", pcb->pid);        
            return EXIT_BLOCK;
        }
    }

    free(decode);
    return NO_BLOCK;
}

char** split_por_bytes(const char* string, size_t bytes, int* cant_partes) {
   /*
    size_t string_len = strlen(string);
    *cant_partes = (string_len + bytes - 1) / bytes;
    int resto = (string_len + bytes - 1) % bytes;

    if(resto == 0){
    printf("cant partes inicial %d \n", *cant_partes);
    char** partes = (char**)malloc((*cant_partes) * sizeof(char*));

    for (int i = 0; i < *cant_partes; i++) {
        partes[i] = (char*)malloc((bytes + 1) * sizeof(char));
        strncpy(partes[i], string + i * bytes, bytes);
        partes[i][bytes] = '\0'; 
    }

    return partes;
    }
    else{
    *cant_partes = *cant_partes + 1;
    printf("cant partes %d \n  ", *cant_partes);
    char** partes = (char**)malloc((*cant_partes) * sizeof(char*));
    for (int i = 0; i < *cant_partes; i++) {
        
        if(i == (*cant_partes - 1)){
            partes[i] = (char*)malloc((resto + 1) * sizeof(char));
            strncpy(partes[i], string + ((i-1) * bytes) + 1, resto);
            partes[i][resto] = '\0'; 
            printf("restoo %d \n", resto);
        }
        else{   
        partes[i] = (char*)malloc((bytes + 1) * sizeof(char));
        strncpy(partes[i], string + i * bytes, bytes);
        partes[i][bytes] = '\0'; 
        }
    }

    }
    */
    size_t string_len = strlen(string);
    *cant_partes = (string_len + bytes - 1) / bytes; // Calcula la cantidad de partes
    int resto = string_len % bytes; // Calcula el resto para la última parte

    printf("Cant partes: %d\n", *cant_partes);

    char** partes = (char**)malloc((*cant_partes) * sizeof(char*));

    // Iterar sobre las partes
    for (int i = 0; i < *cant_partes; i++) {
        size_t current_part_size = (i == (*cant_partes - 1) && resto != 0) ? resto : bytes; // Tamaño de la parte actual
        partes[i] = (char*)malloc((current_part_size + 1) * sizeof(char)); // Reservar memoria para la parte
        strncpy(partes[i], string + i * bytes, current_part_size); // Copiar la parte de la cadena original
        partes[i][current_part_size] = '\0'; // Asegurar que la cadena termina con '\0'
    }

    return partes; // Devolver el arreglo de partes
}
    


void realizar_ciclo_inst(int conexion, t_pcb* pcb, t_log* logger){
   
   t_cpu_blockeo blockeo = NO_BLOCK;
    while(blockeo == NO_BLOCK && !hay_interrupcion)
    {
        t_instruccion* ins = fetch(conexion,pcb);
        log_info(logger, "PID: %d - FETCH - Program counter: <%d>", pcb->pid, pcb->registros->PC);

        t_decode* decodeado = decode(ins);
    
        log_debug(logger, "Numero instruccion: %d", decodeado->op_code);

        blockeo = execute(decodeado,pcb, logger);
        loggear_registros(pcb, logger);
    }

    if(hay_interrupcion == 1) // Fin de quantum
    {
        pcb->motivo_desalojo = 1;
        log_debug(cpu_log, "Envio PCB interrumpido por fin de quantum");
        hay_interrupcion = 0;
    }
    if(blockeo == EXIT_BLOCK)
    {
        pcb->motivo_desalojo = 0;
        log_debug(cpu_log, "Envio PCB terminado.");
    }
    if(blockeo == IO_BLOCK)
    {

    }
    if(blockeo == REC_BLOCK)
    {

    }
}

void loggear_registros(t_pcb* pcb, t_log* logger)
{
    log_debug(logger, "Estado del proceso PID <%d>:", pcb->pid);
    log_debug(logger, "AX: %d", pcb->registros->AX);
    log_debug(logger, "BX: %d", pcb->registros->BX);
    log_debug(logger, "CX: %d", pcb->registros->CX);
    log_debug(logger, "DX: %d", pcb->registros->DX);
    log_debug(logger, "EAX: %d", pcb->registros->EAX);
    log_debug(logger, "EBX: %d", pcb->registros->EBX);
    log_debug(logger, "ECX: %d", pcb->registros->ECX);
    log_debug(logger, "EDX: %d", pcb->registros->EDX);
    log_debug(logger, "SI: %d", pcb->registros->SI);
    log_debug(logger, "DI: %d", pcb->registros->DI);
    log_debug(logger, "PC: %d", pcb->registros->PC);
}