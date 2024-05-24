#include "io.h"

void validar_peticion(char* interfaz_a_validar, char* tiempo, t_pcb* pcb) {
    sem_wait(&sem_contador);
    int tamanio_lista = list_size(interfaces);

    if (tamanio_lista > 0) {
        interfaz* interfaz_encontrada = buscar_interfaz_por_nombre(interfaz_a_validar);

        if (interfaz_encontrada != NULL) {
            enviar_instruccion_a_interfaz(interfaz_encontrada, tiempo);
            interfaz_encontrada->cola_block = queue_push(pcb); // cambiar tema de verificacion de operacion
        } else {
            printf("La interfaz '%s' no existe en la lista.\n", interfaz_a_validar);
            cambiar_a_exit(pcb);
        }
    } else {
        printf("La lista de interfaces está vacía.\n");
    }
}

interfaz* buscar_interfaz_por_nombre(char* nombre_interfaz) {
    int tamanio_lista = list_size(interfaces);
    interfaz* interfaz_encontrada = NULL;

    for (int i = 0; i < tamanio_lista; i++) {
        interfaz* posible_interfaz = (interfaz*)list_get(interfaces, i);
        if (string_equals_ignore_case(posible_interfaz->nombre_interfaz, nombre_interfaz)) {
            interfaz_encontrada = posible_interfaz;
            break;
        }
    }
    return interfaz_encontrada;
}

void enviar_instruccion_a_interfaz(interfaz* interfaz_destino, char* tiempo) {
    t_paquete_instruccion* instruccion_enviar = malloc(sizeof(t_paquete_instruccion));
    instruccion_params* parametros = malloc(sizeof(instruccion_params));

    instruccion_enviar->codigo_operacion = IO_GEN_SLEEP;
    parametros->params.io_gen_sleep_params.unidades_trabajo = strdup(tiempo);

    enviar_instruccion(instruccion_enviar, parametros, interfaz_destino->socket_interfaz);

    free(parametros);
    free(instruccion_enviar);
}
