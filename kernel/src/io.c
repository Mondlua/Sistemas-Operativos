#include "io.h"


void validar_peticion(char* interfaz, char* tiempo) {
    int tamanio_lista = list_size(interfaces);
    if (tamanio_lista > 0) {
        int cont = 0;
        cliente* posible_interfaz;

        while (cont < tamanio_lista) {
            posible_interfaz = (cliente*)list_get(interfaces, cont);

            if (string_equals_ignore_case(posible_interfaz->nombre_cliente, interfaz)) {
                enviar_mensaje(tiempo, posible_interfaz->socket_cliente);
                return; 
            } else {
                cont++;
            }
        }
        printf("La interfaz '%s' no existe en la lista.\n", interfaz);
    } else {
        printf("La lista de interfaces está vacía.\n");
    }
}