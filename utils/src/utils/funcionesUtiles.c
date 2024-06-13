#include "funcionesUtiles.h"

char* int_to_char(int num) {
    char *str;
    int len = snprintf(NULL, 0, "%d", num);
    str = (char *)malloc(len + 1); 
    snprintf(str, len + 1, "%d", num); 
    return str;
}

int find_queue(uint32_t elem, t_queue *cola)
{
    if (queue_is_empty(cola))
    {
        return 0;
    }

    t_link_element *actual = cola->elements->head;

    while (actual != NULL)
    {
        uint32_t *dato = (uint32_t*)actual->data;
        if (*dato == elem)
        {
            return 1;
        }
        actual = actual->next;
    }
    return 0;
}
