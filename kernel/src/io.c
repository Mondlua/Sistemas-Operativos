#include "io.h"


void validar_peticion(char* interfaz_a_validar, char* tiempo) {
    int tamanio_lista = list_size(interfaces);
    printf("tam %i",tamanio_lista);
    interfaz* posible_interfaz = (interfaz*)list_get(interfaces, 0);
    printf("lista en IO: %s ", posible_interfaz->nombre_interfaz);
    if (tamanio_lista > 0) {
        int cont = 0;
        interfaz* posible_interfaz = NULL;

        while (cont < tamanio_lista) {
            posible_interfaz = (interfaz*)list_get(interfaces, cont);

            if (string_equals_ignore_case(posible_interfaz->nombre_interfaz, interfaz_a_validar)) {
                enviar_mensaje(tiempo, posible_interfaz->socket_interfaz);
                printf("MENSAJE ENVIADO %s", tiempo);
                return; 
            } else {
                cont++;
            }
        }
        printf("La interfaz '%s' no existe en la lista.\n", interfaz_a_validar);
    } else {
        printf("La lista de interfaces está vacía.\n");
    }
}