#include "ciclo_inst.h"

t_instruccion* fetch(int conexion_memoria, t_pcb* pcb){
    
    t_instruccion* instruccion;

    enviar_pc(int_to_char(pcb->p_counter),conexion_memoria);

    // SEMAFORO
    sleep(5);

    instruccion = recibir_instruccion_cpu(conexion_memoria);
    
    pcb->p_counter++;

    return instruccion;
}

void eliminar_linea_n(char* linea){
    if(linea[strlen(linea)-1] == '\n'){
        linea[strlen(linea)-1]='\0';
    }
}

instrucciones obtener_instruccion(char *nombre) {
    if (strcmp(nombre, "SET") == 0) {
        return SET;
    } else if (strcmp(nombre, "MOV_IN") == 0) {
        return MOV_IN;
    } else if (strcmp(nombre, "MOV_OUT") == 0) {
        return MOV_OUT;
    } else if (strcmp(nombre, "SUM") == 0) {
        return SUM;
    } else if (strcmp(nombre, "SUB") == 0) {
        return SUB;
    } else if (strcmp(nombre, "JNZ") == 0) {
        return JNZ;
    } else if (strcmp(nombre, "RESIZE") == 0) {
        return RESIZE;
    } else if (strcmp(nombre, "COPY_STRING") == 0) {
        return COPY_STRING;
    } else if (strcmp(nombre, "WAIT") == 0) {
        return WAIT;
    } else if (strcmp(nombre, "SIGNAL") == 0) {
        return SIGNAL;
    } else if (strcmp(nombre, "IO_GEN_SLEEP") == 0) {
        return IO_GEN_SLEEP;
    } else if (strcmp(nombre, "IO_STDIN_READ") == 0) {
        return IO_STDIN_READ;
    } else if (strcmp(nombre, "IO_STDOUT_WRITE") == 0) {
        return IO_STDOUT_WRITE;
    } else if (strcmp(nombre, "IO_FS_CREATE") == 0) {
        return IO_FS_CREATE;
    } else if (strcmp(nombre, "IO_FS_DELETE") == 0) {
        return IO_FS_DELETE;
    } else if (strcmp(nombre, "IO_FS_TRUNCATE") == 0) {
        return IO_FS_TRUNCATE;
    } else if (strcmp(nombre, "IO_FS_WRITE") == 0) {
        return IO_FS_WRITE;
    } else if (strcmp(nombre, "IO_FS_READ") == 0) {
        return IO_FS_READ;
    } else if (strcmp(nombre, "EXIIT") == 0) {
        return EXIIT;
    } else {
        return -1;
    }
}

t_decode* decode(t_instruccion* instruccion){

    char* buffer = (char*) instruccion->buffer->stream;
    eliminar_linea_n(buffer);
    char** arrayIns = string_split(buffer," ");    
    instrucciones ins= obtener_instruccion(arrayIns[0]);
    t_decode* decode = malloc(sizeof(t_decode));
    decode->op_code = ins;
    decode->registroCpu = list_create();
    
    switch(ins){
        case 0:{
        char* registro = arrayIns[1];
        list_add(decode->registroCpu, registro);
        int valor =atoi(arrayIns[2]);
        decode->valor = valor;     
        break;     
        }
        case 1:{
        char* registroDatos = arrayIns[1];
        list_add(decode->registroCpu,registroDatos);
        char* registroDireccion = arrayIns[2];
        list_add(decode->registroCpu,registroDireccion);     
        break;
        }
        case 2:{
        char* registroDireccion = arrayIns[1];
        list_add(decode->registroCpu,registroDireccion);
        char* registroDatos = arrayIns[2];
        list_add(decode->registroCpu,registroDatos);   
        decode->logicaAFisica= true;  
        break;
        }
        case 3:{
        char* registroDestino = arrayIns[1];
        list_add(decode->registroCpu,registroDestino);
        char* registroOrigen= arrayIns[2];
        list_add(decode->registroCpu,registroOrigen);     
        break;
        }
        case 4:{
        char* registroDestino = arrayIns[1];
        list_add(decode->registroCpu,registroDestino);
        char* registroOrigen= arrayIns[2];
        list_add(decode->registroCpu,registroOrigen);
        break;
        }
        case 5:{
        char* registro=arrayIns[1];
        list_add(decode->registroCpu,registro);
        instrucciones ins = atoi(arrayIns[2]);
        decode->instrucciones= ins;
        break;
        }
        case 6:{
        int tamanio = atoi(arrayIns[1]);
        decode->valor= tamanio;
        break;
        }   
        case 7:{
        int tamanio = atoi(arrayIns[1]);
        decode->valor= tamanio;
        break;
        }
        case 8:{
        char *rec;
        rec= arrayIns[1];
        decode->recurso = rec;
        break;
        }
        case 9:{
        char* recu = arrayIns[1];
        decode->recurso=recu;
        break;
        }
        case 10:{
            char* interfaz = strdup(arrayIns[1]);
            decode->interfaz = interfaz;
            int unidadesTrabajo = atoi(arrayIns[2]);
            decode->valor = unidadesTrabajo;
            break;
        }
        case 11:{
            break;
        }
        case 12:{
            break;
        }
        case 13:{
            break;
        }
        case 14:{
            break;
        }
        case 15:{
            break;
        }
        case 16:{
            break;
        }
        case 17:{
            break;
        }
        case 18:{
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
    else if (strcmp(nombre_registro, "BX") == 0) {return regs->BX;}
    else if (strcmp(nombre_registro, "AX") == 0) {return regs->AX;}
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

void execute(t_decode* decode, t_pcb* pcb){
    
    instrucciones ins = decode->op_code;
    switch(ins){
    
        case 0:{
            int valor = decode->valor;
            char* registro_adepositar=(char*)list_get(decode->registroCpu,0);
            asignar_registro(pcb->registros, registro_adepositar, valor);
            break;
         }
        case 1:{}
        case 2:{}
        case 3:{
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
                pcb->p_counter=ins;
            }
        break;
        }
        case 6:{}
        case 7:{}
        case 8:{}
        case 9:{}
        case 10:{
            instruccion_params* parametros =  malloc(sizeof(instruccion_params));;
            parametros->interfaz = strdup(decode->interfaz);;
            parametros->params.io_gen_sleep_params.unidades_trabajo = decode->valor;

            t_paquete_instruccion* paquete = malloc(sizeof(t_paquete_instruccion));;
            paquete->codigo_operacion = IO_GEN_SLEEP;
            enviar_instruccion_a_Kernel(paquete, parametros, kernel_socket);
        
            free(parametros->interfaz); 
            free(parametros);
            free(paquete);
            break;
        }
        case 11:{}
        case 12:{}
        case 13:{}
        case 14:{}
        case 15:{}
        case 16:{}
        case 17:{}
        case 18:{}
    }

}

void realizar_ciclo_inst(int conexion_memoria, t_pcb* pcb){
   t_instruccion* ins = fetch(conexion_memoria,pcb);
   t_decode* decodeado = decode(ins);
   execute(decodeado, pcb);
}
