#include "io.h"


void validar_peticion(char* interfaz_a_validar, char* tiempo) {
    sem_wait(&sem_contador);
    int tamanio_lista = list_size(interfaces);
    if (tamanio_lista > 0) {
        int encontrado=0;
        int cont = 0;
        while (cont < tamanio_lista) {
            interfaz* posible_interfaz = (interfaz*)list_get(interfaces, cont);
            if (string_equals_ignore_case(posible_interfaz->nombre_interfaz, interfaz_a_validar)) {
                t_paquete_instruccion* instruccion_enviar = malloc(sizeof(t_paquete_instruccion));
                instruccion_params* parametros = malloc(sizeof(instruccion_params));
                instruccion_enviar->codigo_operacion = IO_GEN_SLEEP;
                parametros->params.io_gen_sleep_params.unidades_trabajo = strdup(tiempo);
                enviar_instruccion(instruccion_enviar, parametros, posible_interfaz->socket_interfaz);
                free(parametros);
                free(instruccion_enviar);
                encontrado++;
                break;
            } else {
                cont++;
            }
        }
        if(!encontrado){
            printf("La interfaz '%s' no existe en la lista.\n", interfaz_a_validar);
        }
    } else {
        printf("La lista de interfaces está vacía.\n");
    }
}