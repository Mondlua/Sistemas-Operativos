#include "io.h"


pthread_t atender_peticion_IO(char* interfaz, char* tiempo, int server){
    pthread_t io;
    atender_peticion_args* args = malloc(sizeof(atender_peticion_args));
    args->interfaz_solicitada = interfaz;
    args->timer = tiempo;
    args->cliente = server;
    int resultado_creacion = pthread_create(&io, NULL, validar_peticion, (void *)args);
    if (resultado_creacion != 0) {
        fprintf(stderr, "Error: No se pudo crear el hilo. Código de error: %d\n", resultado_creacion);
    return 1;
    }
    return io;
}

void* validar_peticion(void *args){
    atender_peticion_args *p_args = (atender_peticion_args *)args;
    char *interfaz = p_args->interfaz_solicitada;
    char *tiempo = p_args->timer;
    int cliente = p_args->cliente;
    if(!list_is_empty(interfaces)){
        int cont = 0;
        char* resultado;
        while(cont < list_size(interfaces))
        {
            resultado = list_get(interfaces, cont);
            if (string_equals_ignore_case(resultado, interfaz))
            {
                printf("La interfaz '%s' existe en la lista.\n", interfaz);
                enviar_mensaje(tiempo, cliente);
                break;
            }
            else {
            printf("La interfaz '%s' no existe en la lista.\n", interfaz);
            cont++;
            }
      
        }
    }
    return NULL;
}