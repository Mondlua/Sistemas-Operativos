#include "io.h"


/*pthread_t atender_peticion_IO(char* interfaz, char* tiempo, int server) {
    pthread_t io;
    atender_peticion_args* args = malloc(sizeof(atender_peticion_args));
    args->interfaz_solicitada = interfaz;
    args->timer = tiempo;
    args->cliente = server;

    int resultado_creacion = pthread_create(&io, NULL, validar_peticion, (void *)args);
    if (resultado_creacion != 0) {
        fprintf(stderr, "Error: No se pudo crear el hilo. Código de error: %d\n", resultado_creacion);
        free(args); // Liberar memoria en caso de error
        return 0; // Devolver un valor indicando un error
    }

    return io;
}

void* validar_peticion(void *args) {
    atender_peticion_args *p_args = (atender_peticion_args *)args;
    char *interfaz = p_args->interfaz_solicitada;
    char *tiempo = p_args->timer;
    int cliente = p_args->cliente;

    // Lógica para validar la petición y enviar mensajes
    if (!list_is_empty(interfaces)) {
        int cont = 0;
        char* resultado;
        while (cont < list_size(interfaces)) {
            resultado = list_get(interfaces, cont);
            if (string_equals_ignore_case(resultado, interfaz)) {
                enviar_mensaje(tiempo, cliente);
                break;
            }
            else {
            printf("La interfaz '%s' no existe en la lista.\n", interfaz);
            cont++;
            }
      
        }
    }
    free(p_args);
    return NULL;
}*/


void validar_peticion(char* interfaz, char* tiempo) {
    int tamano_lista = list_size(interfaces);
    printf("int %i  ", fd_interfaz);
    if (tamano_lista > 0) {
        int cont = 0;
        char* resultado;

        while (cont < tamano_lista) {
            resultado = list_get(interfaces, cont);

            if (string_equals_ignore_case(resultado, interfaz)) {
             //   enviar_mensaje(tiempo, fd_interfaz);
                return;  // Salir de la función después de enviar el mensaje
            } else {
                cont++;
            }
        }
        printf("La interfaz '%s' no existe en la lista.\n", interfaz);
    } else {
        printf("La lista de interfaces está vacía.\n");
    }
}