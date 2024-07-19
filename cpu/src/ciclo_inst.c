#include "ciclo_inst.h"

char* fetch(int conexion, t_pcb* pcb){
    
    enviar_pid(int_to_char(pcb->pid), conexion);
    enviar_pc(int_to_char(pcb->registros->PC),conexion);
    int x = recibir_operacion(conexion);
    char* ins = recibir_mensaje(conexion, cpu_log);
    pcb->registros->PC++;

    return ins;
}

char* eliminar_linea_n(char* instruccion) {
    int len = strlen(instruccion);
    if (len > 0 && instruccion[len - 1] == '\n') {
        instruccion[len - 1] = '\0';
    }
    return instruccion;
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

t_decode* decode(char* buffer){

    //char* buffer; //= (char*) instruccion->buffer->stream;
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
        char* interfaz = strdup(arrayIns[1]);
        decode->interfaz = interfaz;
        char* archivo = strdup(arrayIns[2]);
        decode->archivo = archivo;
        break;
        }
        case IO_FS_DELETE:{
        char* interfaz = strdup(arrayIns[1]);
        decode->interfaz = interfaz;
        char* archivo = strdup(arrayIns[2]);
        decode->archivo = archivo;
        break;
        }
        case IO_FS_TRUNCATE:{
        char* interfaz = strdup(arrayIns[1]);
        decode->interfaz = interfaz;
        char* archivo = strdup(arrayIns[2]);
        decode->archivo = archivo;
        char* registroTamaño = strdup(arrayIns[3]);
        list_add(decode->registroCpu, registroTamaño);
        break;
        }
        case IO_FS_WRITE:{
        char* interfaz = strdup(arrayIns[1]);
        decode->interfaz = interfaz;
        char* archivo = strdup(arrayIns[2]);
        decode->archivo = archivo;
        char* registroDireccion = strdup(arrayIns[3]);
        list_add(decode->registroCpu, registroDireccion);
        char* registroTamaño = strdup(arrayIns[4]);
        list_add(decode->registroCpu, registroTamaño);
        char* registroPuntero = strdup(arrayIns[5]);
        list_add(decode->registroCpu, registroPuntero);
        break;
        }
        case IO_FS_READ:{
        char* interfaz = strdup(arrayIns[1]);
        decode->interfaz = interfaz;
        char* archivo = strdup(arrayIns[2]);
        decode->archivo = archivo;
        char* registroDireccion = strdup(arrayIns[3]);
        list_add(decode->registroCpu, registroDireccion);
        char* registroTamaño = strdup(arrayIns[4]);
        list_add(decode->registroCpu, registroTamaño);
        char* registroPuntero = strdup(arrayIns[5]);
        list_add(decode->registroCpu, registroPuntero);
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
    t_cpu_blockeo ret;
    ret.blockeo = NO_BLOCK;
    ret.instrucciones = NULL;
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
            uint32_t pid=pcb->pid;

            int tam_mensaje = sizeof(tamanio)+sizeof(num_frame)+sizeof(desplazamiento)+sizeof(pid); 
            char* mensaje = malloc(tam_mensaje);
            sprintf(mensaje, "%d/%d/%d/%u", tamanio,num_frame,desplazamiento,pid); 

            enviar_a_mem(conexion_memoria_cpu, mensaje, PED_LECTURA);

            log_info(logger, "PID: %d - Ejecutando: MOV_IN %s %s", pcb->pid, registro_datos, registro_direccion);        

            char* tam = strcat(int_to_char(tamanio), "/");
            char* tamframe = strcat(tam, int_to_char(num_frame));
            char* tamframe1 = strcat(tamframe, "/");
            char* enviar = strcat(tamframe1, int_to_char(desplazamiento));
            //enviar_pedido_lectura(conexion_memoria_cpu, enviar); TODO: Agregar esto

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

            log_info(logger, "PID: %d - Ejecutando: MOV_OUT %s %s", pcb->pid, registro_datos, registro_direccion);        

            uint8_t valor = (uint8_t) obtener_valor_registro(pcb->registros, registro_datos);
            int tamanio;
          //  printf ("num a escribir %d\n", valor);
            if(obtener_tipo_registro(registro_datos) == 8){
                tamanio =1;
            }
            else{
                tamanio = 4;
            }

            char* aescribir = malloc(sizeof(char*));
            aescribir= int_to_char(valor);

            int cant_pags = tamanio/tam_pag;

            if(cant_pags <=1){        
                t_dir_fisica* dir_fisica = mmu(dir_logica, pcb->pid);

                int num_frame = dir_fisica->nro_frame;
                int desplazamiento = dir_fisica->desplazamiento;

                int tam_mensaje = sizeof(tamanio)+sizeof(aescribir)+sizeof(num_frame)+sizeof(desplazamiento)+sizeof(pcb->pid); 
                char* mensaje = malloc(tam_mensaje);
                sprintf(mensaje, "%d/%s/%d/%d/%d", tamanio,aescribir,num_frame,desplazamiento,pcb->pid);     
                
                enviar_a_mem(conexion_memoria_cpu, mensaje,PED_ESCRITURA);
                int i = recibir_operacion(conexion_memoria_cpu);
                char* frame_siguiente = recibir_mensaje(conexion_memoria_cpu,cpu_log);
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

            log_info(logger, "PID: %d - Ejecutando: SUM %s %s", pcb->pid, registroDestino, registroOrigen);        

            uint8_t valor2 = (uint8_t) obtener_valor_registro(pcb->registros, registroDestino);
            uint8_t valor1 = (uint8_t) obtener_valor_registro(pcb->registros, registroOrigen);
            uint8_t suma = valor1 + valor2;
            asignar_registro(pcb->registros, registroDestino, suma);
            break;
        }
        case SUB:{
            char* registroDestino=(char*)list_get(decode->registroCpu, 0);
            char* registroOrigen=(char*)list_get(decode->registroCpu, 1);

            log_info(logger, "PID: %d - Ejecutando: SUB %s %s", pcb->pid, registroDestino, registroOrigen);        

            uint8_t valor2 = (uint8_t) obtener_valor_registro(pcb->registros, registroDestino);
            uint8_t valor1 = (uint8_t) obtener_valor_registro(pcb->registros, registroOrigen);
            uint8_t resta = valor2 - valor1;
            asignar_registro(pcb->registros, registroDestino, resta);
            break;
        }
        case JNZ:{
            char* registro = (char*)list_get(decode->registroCpu, 0);
            uint32_t instruccion = (uint32_t)list_get(decode->registroCpu, 1);
            
            log_info(cpu_log, "PID: %d - Ejecutando: JNZ %s %d", pcb->pid, registro, instruccion);
            
            uint8_t valor=obtener_valor_registro(pcb->registros, registro);
            if(valor!=0){
                instrucciones ins= decode->instrucciones;

                pcb->registros->PC= valor;

               // asignar_registro(pcb->registros, "PC", valor );

            }
            break;
        }
        case RESIZE:{
            int tamanio_resize = decode->valor;


            log_info(cpu_log, "PID: %d - Ejecutando: RESIZE %d", pcb->pid, tamanio_resize);

            char* tamanio = int_to_char(tamanio_resize);
            char* pid_char = int_to_char(pcb->pid);
            char* mensaje1 = strcat(pid_char,"/");
            char* mensaje = strcat(mensaje1,tamanio);
            //enviar_pedido_resize_tampid(conexion_memoria_cpu, mensaje); TODO: agregar esto

            break;
        }
        case COPY_STRING:{
            int bytes = decode->valor;
            int cant_char = bytes / sizeof(char*);
            uint32_t string_apuntado =obtener_valor_registro(pcb->registros, "SI");
            uint32_t dir_apunt=obtener_valor_registro(pcb->registros, "DI");
            t_dir_fisica* dirstring= mmu(string_apuntado, pcb->pid);
            t_dir_fisica* dir_apuntada = mmu(dir_apunt, pcb->pid);
            int d1=dir_apuntada->nro_frame;
            int d2=dir_apuntada->desplazamiento;
            int d3=dirstring->nro_frame;
            int d4=dirstring->desplazamiento;
            char* mensaje = malloc(sizeof(d1)+sizeof(d2)+sizeof(d3)+sizeof(d4)+sizeof(cant_char));
            sprintf(mensaje, "%d/%d/%d/%d/%d", d1,d2,d3,d4,cant_char);      //ver de implementar en demas    
            enviar_a_mem(conexion_memoria_cpu, mensaje,CPY_STRING);
           
            break;
        }

        case WAIT:{
            ret.nombre_recurso = strdup(decode->recurso); // TODO: Capaz necesita un free

            log_info(cpu_log, "PID: %d - Ejecutando: WAIT %s", pcb->pid, ret.nombre_recurso);

            ret.blockeo = REC_BLOCK_WAIT;
            return ret; 
        }
        case SIGNAL:{
            ret.nombre_recurso = strdup(decode->recurso); // TODO: Capaz necesita un free

            log_info(cpu_log, "PID: %d - Ejecutando: SIGNAL %s", pcb->pid, ret.nombre_recurso);

            ret.blockeo = REC_BLOCK_SIGNAL;
            return ret;
        }
        case IO_GEN_SLEEP:{ 
            instruccion_params* parametros =  malloc(sizeof(instruccion_params));
            parametros->interfaz = strdup(decode->interfaz);
            parametros->params.io_gen_sleep.unidades_trabajo = decode->valor;

            log_info(cpu_log, "PID: %d - Ejecutando: IO_GEN_SLEEP %s %d", pcb->pid, parametros->interfaz, parametros->params.io_gen_sleep.unidades_trabajo);

            ret.io_opcode = IO_GEN_SLEEP;
            ret.blockeo = IO_BLOCK;
            ret.instrucciones = parametros;
            return ret;
            // En kernel agregar un recv para recibir el instruccion_params y operar en base a eso tratando de usar las cosas que hizo zoe.
        }
        case IO_STDIN_READ:{

            instruccion_params* parametros =  malloc(sizeof(instruccion_params));
            parametros->interfaz = strdup(decode->interfaz);
            char* registro_direccion = (char*)list_get(decode->registroCpu, 0);
            char* registro_tamaño = (char*)list_get(decode->registroCpu, 1);

            log_info(cpu_log, "PID: %d - Ejecutando IO_STDIN_READ %s %s %s", pcb->pid, parametros->interfaz, registro_direccion, registro_tamaño);

            int dir_logica =(int)obtener_valor_registro(pcb->registros, registro_direccion);

            t_dir_fisica* dir_fisica = mmu(dir_logica, pcb->pid);

            parametros->registro_direccion = dir_fisica;
            parametros->registro_tamanio = (uint32_t)obtener_valor_registro(pcb->registros, registro_tamaño);
          
            ret.io_opcode = IO_STDIN_READ;
            ret.blockeo = IO_BLOCK;
            ret.instrucciones = parametros;
            return ret;
        }
        case IO_STDOUT_WRITE:{

            instruccion_params* parametros =  malloc(sizeof(instruccion_params));
            parametros->interfaz = strdup(decode->interfaz);
            char* registro_direccion = (char*)list_get(decode->registroCpu, 0);
            char* registro_tamaño = (char*)list_get(decode->registroCpu, 1);

            log_info(cpu_log, "PID: %d - Ejecutando IO_STDOUT_WRITE %s %s %s", pcb->pid, parametros->interfaz, registro_direccion, registro_tamaño);

            int dir_logica =(int)obtener_valor_registro(pcb->registros, registro_direccion);

            t_dir_fisica* dir_fisica = mmu(dir_logica, pcb->pid);

            parametros->registro_direccion = dir_fisica;
            parametros->registro_tamanio = (uint32_t)obtener_valor_registro(pcb->registros, registro_tamaño);
            
            ret.io_opcode = IO_STDOUT_WRITE;
            ret.blockeo = IO_BLOCK;
            ret.instrucciones = parametros;
            return ret;
        }
        case IO_FS_CREATE:{
            
            instruccion_params* parametros = malloc(sizeof(instruccion_params));
            parametros->interfaz = strdup(decode->interfaz);
            parametros->params.io_fs.nombre_archivo = strdup(decode->archivo);
    
            // t_paquete_instruccion* paquete = malloc(sizeof(t_paquete_instruccion));
            // paquete->codigo_operacion = IO_FS_CREATE;
    
            // enviar_instruccion_a_Kernel(paquete, parametros, kernel_socket);
            // free(parametros->interfaz);
            // free(parametros->params.io_fs.nombre_archivo);
            // free(parametros);
            // free(paquete);
            ret.blockeo = IO_BLOCK;
            ret.instrucciones = parametros;
            ret.io_opcode = IO_FS_CREATE;
            return ret;
        }
        case IO_FS_DELETE:{ 
            //enviar_motivo(BLOCK_IO, kernel_socket);
            instruccion_params* parametros = malloc(sizeof(instruccion_params));
            parametros->interfaz = strdup(decode->interfaz);
            parametros->params.io_fs.nombre_archivo = strdup(decode->archivo);
    
            // t_paquete_instruccion* paquete = malloc(sizeof(t_paquete_instruccion));
            // paquete->codigo_operacion = IO_FS_DELETE;
    
            // enviar_instruccion_a_Kernel(paquete, parametros, kernel_socket);
            // free(parametros->interfaz);
            // free(parametros->params.io_fs.nombre_archivo);
            // free(parametros);
            // free(paquete);
            ret.blockeo = IO_BLOCK;
            ret.io_opcode = IO_FS_DELETE;
            ret.instrucciones = parametros;
            return ret;
        }
        case IO_FS_TRUNCATE:{ 
           // enviar_motivo(BLOCK_IO, kernel_socket);
            instruccion_params* parametros = malloc(sizeof(instruccion_params));
            parametros->interfaz = strdup(decode->interfaz);
            parametros->params.io_fs.nombre_archivo = strdup(decode->archivo);
            char* registro_tamanio = list_get(decode->registroCpu, 0);
            parametros->registro_tamanio = (uint32_t)obtener_valor_registro(pcb->registros, registro_tamanio);
    
            // t_paquete_instruccion* paquete = malloc(sizeof(t_paquete_instruccion));
            // paquete->codigo_operacion = IO_FS_TRUNCATE;
    
            // enviar_instruccion_a_Kernel(paquete, parametros, kernel_socket);
            // free(parametros->interfaz);
            // free(parametros->params.io_fs.nombre_archivo);
            // free(parametros);
            // free(paquete);
            ret.blockeo = IO_BLOCK;
            ret.io_opcode = IO_FS_TRUNCATE;
            ret.instrucciones = parametros;
            return ret;
        }
        case IO_FS_WRITE:{ //IO_FS_WRITE
            //enviar_motivo(BLOCK_IO, kernel_socket);
            instruccion_params* parametros = malloc(sizeof(instruccion_params));
            parametros->interfaz = strdup(decode->interfaz);
            parametros->params.io_fs.nombre_archivo = strdup(decode->archivo);
            char* registro_direccion = list_get(decode->registroCpu, 0);
            char* registro_tamano = list_get(decode->registroCpu, 1);
            char* registro_puntero_archivo = list_get(decode->registroCpu, 2);
            int dir_logica =(int)obtener_valor_registro(pcb->registros, registro_direccion);

            t_dir_fisica* dir_fisica = mmu(dir_logica, pcb->pid);
            parametros->registro_direccion = dir_fisica;
            parametros->registro_tamanio = (uint32_t)obtener_valor_registro(pcb->registros, registro_tamano);
            parametros->params.io_fs.registro_puntero_archivo = (off_t)obtener_valor_registro(pcb->registros, registro_puntero_archivo);
    
            // t_paquete_instruccion* paquete = malloc(sizeof(t_paquete_instruccion));
            // paquete->codigo_operacion = IO_FS_WRITE;
    
            // enviar_instruccion_a_Kernel(paquete, parametros, kernel_socket);
            // free(parametros->interfaz);
            // free(parametros->params.io_fs.nombre_archivo);
            // free(parametros);
            // free(paquete);
            ret.blockeo = IO_BLOCK;
            ret.io_opcode = IO_FS_WRITE;
            ret.instrucciones = parametros;
            return ret;
        }
        case IO_FS_READ:{ //IO_FS_READ
           // enviar_motivo(BLOCK_IO, kernel_socket);
            instruccion_params* parametros = malloc(sizeof(instruccion_params));
            parametros->interfaz = strdup(decode->interfaz);
            parametros->params.io_fs.nombre_archivo = strdup(decode->archivo);
            char* registro_direccion = list_get(decode->registroCpu, 0);
            char* registro_tamano = list_get(decode->registroCpu, 1);
            char* registro_puntero_archivo = list_get(decode->registroCpu, 2);
            int dir_logica =(int)obtener_valor_registro(pcb->registros, registro_direccion);

            t_dir_fisica* dir_fisica = mmu(dir_logica, pcb->pid);
            parametros->registro_direccion = dir_fisica;
            parametros->registro_tamanio = (uint32_t)obtener_valor_registro(pcb->registros, registro_tamano);
            parametros->params.io_fs.registro_puntero_archivo = (off_t)obtener_valor_registro(pcb->registros, registro_puntero_archivo);
    
            // t_paquete_instruccion* paquete = malloc(sizeof(t_paquete_instruccion));
            // paquete->codigo_operacion = IO_FS_READ;
    
            // enviar_instruccion_a_Kernel(paquete, parametros, kernel_socket);
            // free(parametros->interfaz);
            // free(parametros->params.io_fs.nombre_archivo);
            // free(parametros);
            // free(paquete);
            ret.blockeo = IO_BLOCK;
            ret.io_opcode = IO_FS_READ;
            ret.instrucciones = parametros;
            return ret;
        }
        case EXIIT:{
            log_info(logger, "PID: %d - Ejecutando: EXIT", pcb->pid);
            ret.blockeo = EXIT_BLOCK;
            return ret;
        }
    }

    free(decode);
    return ret;
}

/*
char** split_por_bytes(const char* string, size_t bytes, int* cant_partes) {
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
}*/
    
void realizar_ciclo_inst(int conexion, t_pcb* pcb, t_log* logger, int socket_cliente, pthread_mutex_t lock_interrupt){
   
   t_cpu_blockeo blockeo;
   blockeo.blockeo = NO_BLOCK;
   pthread_mutex_unlock(&lock_interrupt);
    while(blockeo.blockeo == NO_BLOCK && !hay_interrupcion)
    {
        log_info(logger, "PID: %d - FETCH - Program counter: <%d>", pcb->pid, pcb->registros->PC);
        char* ins = fetch(conexion,pcb);
        
        t_decode* decodeado = decode(ins);
    
        log_debug(logger, "Numero instruccion: %d", decodeado->op_code);

        blockeo = execute(decodeado,pcb, logger);
        loggear_registros(pcb, logger);
    }

    pthread_mutex_lock(&lock_interrupt);
    if(hay_interrupcion == 1 && blockeo.blockeo == NO_BLOCK) // Fin de quantum
    {
        hay_interrupcion = 0;
        pthread_mutex_unlock(&lock_interrupt);
        pcb->motivo_desalojo = 1;
        log_debug(cpu_log, "Envio PCB interrumpido por fin de quantum");
        enviar_pcb(pcb, socket_cliente);
        return;
    }
    if(blockeo.blockeo == EXIT_BLOCK)
    {
        hay_interrupcion = 0;
        pthread_mutex_unlock(&lock_interrupt);
        pcb->motivo_desalojo = 0;
        log_debug(cpu_log, "Envio PCB terminado.");
        enviar_pcb(pcb, socket_cliente);
        return;
    }
    if(blockeo.blockeo == IO_BLOCK)
    {
        hay_interrupcion = 0;
        pthread_mutex_unlock(&lock_interrupt);
        pcb->motivo_desalojo = 2;
        log_debug(cpu_log, "Envio PCB desalojado por solicitud a interfaz");
        enviar_pcb(pcb, socket_cliente);

        t_paquete_instruccion* paquete = malloc(sizeof(t_paquete_instruccion));
        paquete->codigo_operacion = blockeo.io_opcode;
        log_debug(cpu_log, "OPCODE enviado: %d", paquete->codigo_operacion);
        enviar_instruccion_a_Kernel(paquete, blockeo.instrucciones, socket_cliente);
        log_debug(cpu_log, "Instruccion enviada al socket: %d.", socket_cliente);
        free(blockeo.instrucciones->interfaz);
        free(blockeo.instrucciones);
        free(paquete);
        return;
    }
    if(blockeo.blockeo == REC_BLOCK_WAIT)
    {
        hay_interrupcion = 0;
        pthread_mutex_unlock(&lock_interrupt);
        pcb->motivo_desalojo = 3;
        log_debug(cpu_log, "Envio PCB desalojado por WAIT");
        enviar_pcb(pcb, socket_cliente);

        log_debug(cpu_log, "Envio el nombre del recurso afectado");
        enviar_mensaje(blockeo.nombre_recurso, socket_cliente);
        // enviar_nombre_recurso(blockeo.nombre_recurso, socket_cliente);
    }
    if(blockeo.blockeo == REC_BLOCK_SIGNAL)
    {
        hay_interrupcion = 0;
        pthread_mutex_unlock(&lock_interrupt);
        pcb->motivo_desalojo = 4;
        log_debug(cpu_log, "Envio PCB desalojado por SIGNAL");
        enviar_pcb(pcb, socket_cliente);

        log_debug(cpu_log, "Envio el nombre del recurso afectado");
        enviar_mensaje(blockeo.nombre_recurso, socket_cliente);
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