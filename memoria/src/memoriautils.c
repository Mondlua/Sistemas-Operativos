#include "memoriautils.h"

int tam_pagina;
int tam_memoria;
t_list* tabla_pags;
void* memoria;
t_bitarray* bitarray;


void escribir_en_mem(char* aescribir, t_dir_fisica* dir_fisica){
    //DIRECCION FISICA = FRAME Y DESPLAZAMIENTO
    int tam_bytes = sizeof(aescribir);
    int pag_necesarias = tam_bytes / tam_pagina;

    int nro_frame = dir_fisica->nro_frame;
    int desplazamiento = dir_fisica->desplazamiento;

    void* espacio_de_mem = (char*)memoria + (nro_frame * tam_pagina) + desplazamiento;
    memcpy(espacio_de_mem , aescribir, tam_bytes); 
}

char* leer_en_mem(int tamanio, t_dir_fisica* dir_fisica){

    int nro_frame = dir_fisica->nro_frame;
    int desplazamiento = dir_fisica->desplazamiento;
    void* espacio_de_mem = (char*)memoria + (nro_frame * tam_pagina) + desplazamiento;

    char* leido = malloc(tamanio);
    memcpy(leido, espacio_de_mem, tamanio);
    return leido;
}

t_tabla* buscar_por_pid_return(uint32_t pid) {

    if(tabla_pags != NULL){
    t_link_element* current = tabla_pags->head;
    while (current != NULL) {
        t_tabla* current_tabla = (t_tabla*)current->data;
        if (current_tabla->pid == pid) {
            return current_tabla; 
        }
        current = current->next;
    }
    }
    return NULL; 
}
bool buscar_por_pid_bool(uint32_t pid) {
    t_link_element* current = tabla_pags->head;
    while (current != NULL) {
        t_tabla* current_tabla = (t_tabla*)current->data;
        if (current_tabla->pid == pid) {
            return true;
        }
        current = current->next;
    }
    return false; 
}

char* cortar_string(char* cadena, int longitud){
    char* nueva_cadena = (char*)malloc((longitud + 1) * sizeof(char));
    if (nueva_cadena == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(nueva_cadena, cadena, longitud);
    nueva_cadena[longitud] = '\0';
    return nueva_cadena;
}

char* cortar_string_final(char* cadena, int longitud) {
    int longitud_cadena = (int) strlen(cadena);
    int inicio_subcadena = longitud_cadena - longitud;

    char* subcadena = (char*)malloc((longitud + 1) * sizeof(char));
    if (subcadena == NULL) {
        perror("Error al reservar memoria para el substring");
        exit(EXIT_FAILURE);
    }

    strncpy(subcadena, cadena + inicio_subcadena, longitud);
    subcadena[longitud] = '\0';

    return subcadena;
}

int buscar_frame_disp(t_bitarray* bit, int tam ){

    bool encontre = false;
    int i = 0;
    
    while((i<tam) && (!encontre)){
        if(bitarray_test_bit(bit,i)==0){
            encontre=true;
        }
        else{
            i++;
        }
        
    }
    return i; 
}



