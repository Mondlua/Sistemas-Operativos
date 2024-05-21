#include "ciclo_inst.h"

t_instruccion* fetch(t_pcb* pcb, int conexion_memoria){
    
    t_instruccion* instruccion;

    enviar_pc(int_to_char(pcb->p_counter),conexion_memoria);
    instruccion = recibir_instruccion_cpu(conexion_memoria);

    printf("Recibi la INSTRUCCION %s", instruccion->buffer->stream);
    
    pcb->p_counter++;
    return instruccion;
}

t_decode* decode(t_instruccion* instruccion){

    instrucciones ins;
    char* buffer = (char*) instruccion->buffer->stream;
    char** arrayIns = malloc(sizeof(char*)*4);
    arrayIns = string_split(buffer," ");
    ins = obtener_instruccion(arrayIns[0]);
    t_decode* decode = malloc(sizeof(t_decode));
    decode->op_code = ins;
    decode->registroCpu = list_create();
    
    switch(ins){
        case 0:{
        cpu_registros* registro = arrayIns[1];
        list_add(decode->registroCpu, registro);
        int valor =atoi(arrayIns[2]);
        decode->valor = valor;     
        break;     
        }
        case 1:{
        cpu_registros* registroDatos = arrayIns[1];
        list_add(decode->registroCpu,registroDatos);
        cpu_registros* registroDireccion = arrayIns[2];
        list_add(decode->registroCpu,registroDireccion);     
        break;
        }
        case 2:{
        cpu_registros* registroDireccion = arrayIns[1];
        list_add(decode->registroCpu,registroDireccion);
        cpu_registros* registroDatos = arrayIns[2];
        list_add(decode->registroCpu,registroDatos);   
        decode->logicaAFisica= true;  
        break;
        }
        case 3:{
        cpu_registros* registroDestino = arrayIns[1];
        list_add(decode->registroCpu,registroDestino);
        cpu_registros* registroOrigen= arrayIns[2];
        list_add(decode->registroCpu,registroOrigen);     
        break;
        }
        case 4:{
        cpu_registros* registroDestino = arrayIns[1];
        list_add(decode->registroCpu,registroDestino);
        cpu_registros* registroOrigen= arrayIns[2];
        list_add(decode->registroCpu,registroOrigen);
        break;
        }
        case 5:{
        instrucciones ins = obtener_instruccion(arrayIns[1]);
        decode->ins= ins;
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
   
   return decode;
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