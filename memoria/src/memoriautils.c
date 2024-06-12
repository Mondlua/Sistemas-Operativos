#include "memoriautils.h"

int tam_pagina;
int tam_memoria;
t_list* tabla_pags;
void* memoria;
t_bitarray* bitarray;


void cargar_a_mem(char* instruccion, uint32_t pid){ // Mejorar segun dir fisica y ver si nos pasamos

    int tam_instruccion = strlen(instruccion) + 1;
    log_info(memoria_log, "tam: %d", tam_instruccion);
    int num_frames = tam_memoria/tam_pagina;
    log_info(memoria_log, "frames: %d", num_frames);
    log_info(memoria_log, "tam pag: %d", tam_pagina);

    t_tabla* tabla_del_pid = buscar_por_pid_return(pid);
   /*
   if(!buscar_por_pid_bool(pid)){
    t_tabla* tabla = malloc(sizeof(t_tabla));
    tabla->pid = pid;
    tabla->tabla = list_create();
    list_add(tabla_pags, tabla);
    tabla_del_pid = buscar_por_pid_return(pid);
    log_info(memoria_log, "cree tabla del pid");
   }*/
    //ENTRA EN UNA PAGINA
    if(tam_instruccion <= tam_pagina){
        log_info(memoria_log, "entre");
        int frame_disp = buscar_frame_disp(bitarray,num_frames);

        memcpy((memoria + (frame_disp*tam_pagina)) ,instruccion, tam_instruccion); 
        bitarray_set_bit(bitarray, frame_disp);
        int *frame_disp_ptr = malloc(sizeof(int)); //Liberar luego
         *frame_disp_ptr = frame_disp;
        log_info(memoria_log, "Cargue PAG en FRAME:%d", frame_disp);
        list_add(tabla_del_pid->tabla, frame_disp_ptr);
    }
    else{ //LO CARGA EN DOS FALTA AGREGAR EN 3 , 4 , 5 PAGS
        log_info(memoria_log, "entre1");
        int frame_disp = buscar_frame_disp(bitarray,num_frames);
        bitarray_set_bit(bitarray, frame_disp);
        int frame_disp2 = buscar_frame_disp(bitarray,num_frames);
        bitarray_set_bit(bitarray, frame_disp2);

        char* nuevo1 = cortar_string(instruccion, tam_pagina/sizeof(char));
        //memcpy((memoria + (frame_disp*tam_pagina)) ,nuevo1, sizeof(nuevo1));
        memcpy((memoria + (frame_disp * tam_pagina)), nuevo1, tam_pagina);
        int *frame_disp_ptr = malloc(sizeof(int)); //Liberar luego
        *frame_disp_ptr = frame_disp;
        list_add(tabla_del_pid->tabla, frame_disp_ptr);


        char* nuevo2 = cortar_string(instruccion, (tam_instruccion - tam_pagina)/sizeof(char));
        //memcpy((memoria + (frame_disp2*tam_pagina)) ,nuevo2, sizeof(nuevo2));
        memcpy((memoria + (frame_disp2 * tam_pagina)), nuevo2, tam_instruccion - tam_pagina);
        int *frame_disp_ptr2 = malloc(sizeof(int)); //Liberar luego 
        *frame_disp_ptr2 = frame_disp2;
        list_add(tabla_del_pid->tabla, frame_disp_ptr2);   
        printf("Cargue PAG en FRAME:%d", frame_disp);
        printf("Cargue PAG en FRAME:%d", frame_disp2);
        free(nuevo1);
        free(nuevo2);
    }
}


t_tabla* buscar_por_pid_return(uint32_t pid) {
    t_link_element* current = tabla_pags->head;
    while (current != NULL) {
        t_tabla* current_tabla = (t_tabla*)current->data;
        if (current_tabla->pid == pid) {
            return current_tabla; 
        }
        current = current->next;
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