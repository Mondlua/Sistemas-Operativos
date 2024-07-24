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


//INCLUYE EL END
char* decstring(const char* str, int start, int end) {
    //posicion inicial arranca en 0 

    if (start < 0 || end < 0 || start > end || end >= strlen(str)) {
        return NULL;
    }

    int length = end - start + 1;

    char* substring = (char*)malloc((length + 1) *sizeof(char));
    if (substring == NULL) {
        return NULL; 
    }

    strncpy(substring, str + start, length);
    substring[length] = '\0';
    return substring;
}