#include "memoriautils.h"

int tam_pagina;
int tam_memoria;
t_list* tabla_pags;
void* memoria;
t_bitarray* bitarray;
t_bitarray* escrito;

void escribir_en_mem(char* aescribir, t_dir_fisica* dir_fisica, int tamanio ){
    //DIRECCION FISICA = FRAME Y DESPLAZAMIENTO
    int tam_bytes = tamanio;
    int nro_frame = dir_fisica->nro_frame;
    int desplazamiento = dir_fisica->desplazamiento;

    if (tamanio == 1){ //instrucciones normales//uint8
        uint8_t escribir = (uint8_t) atoi(aescribir);
        memcpy((char*)memoria + (nro_frame * tam_pagina) + desplazamiento , &escribir, tam_bytes); 
        //printf("Escribi en Nro Frame <%d> y Desp <%d>: <%u>\n", nro_frame, desplazamiento, escribir);
    }
    if(tamanio ==4){//uint32
        uint32_t escribir = (uint32_t)atoi(aescribir);
        memcpy((char*)memoria + (nro_frame * tam_pagina) + desplazamiento , &escribir, tam_bytes);
        //printf("Escribi en Nro Frame <%d> y Desp <%d>: <%u>\n", nro_frame, desplazamiento, escribir);
    }
   else{
    
        int cant_pags_necesarias = tamanio/tam_pagina;
        int desplazamiento = tamanio % tam_pagina;
        int resto=0;
        int cont = 0;
        int pid; //tiene q recibir el pid
        if(desplazamiento != 0){
            cant_pags_necesarias++;
            resto=1;
        }
        char** arr;
        if(puede_escribir(pid,nro_frame, cant_pags_necesarias)){
           arr=dividir_str_segun_pags(aescribir, cant_pags_necesarias,resto);
            memcpy((char*)memoria + (nro_frame * tam_pagina) + desplazamiento , arr[0], tam_pagina);
            bitarray_set_bit(escrito, nro_frame);
            for(int i=1; i<cant_pags_necesarias+resto;i++){
                int frame_sig=frame_sig_disp(pid, nro_frame);
                memcpy((char*)memoria + (frame_sig * tam_pagina) + desplazamiento , arr[i], tam_pagina);
                bitarray_set_bit(escrito, frame_sig);
                nro_frame=frame_sig;
            }
           }
        
        else{
          //  out of mem
        }
        
        memcpy((char*)memoria + (nro_frame * tam_pagina) + desplazamiento , aescribir, tam_bytes);
        //printf("Escribi en Nro Frame <%d> y Desp <%d>: <%s>\n", nro_frame, desplazamiento, aescribir);
    }

}

char** dividir_str_segun_pags(char* str, int cantpags, int resto){
    int contador=0;
    int pos=0;
    char** arr=(char**)malloc(sizeof(char*)*(cantpags+resto));
    if(resto==0){
    while(contador<cantpags){
        char* substring= decstring(str,pos,pos+tam_pagina);
        arr[contador]=substring;
        pos+=tam_pagina;
        contador++;
    }
    }
    else{
       while(contador<cantpags-1){
        char* substring= decstring(str,pos,pos+tam_pagina);
        arr[contador]=substring;
        pos+=tam_pagina;
        contador++;
        } 
        char* subs=decstring(str,pos,strlen(str)-1);
        arr[contador]=subs;
    }
    return arr;
}


//hacer dos funciones para escribir una por fs y otra de mov
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
char* leer_en_mem(int tamanio, t_dir_fisica* dir_fisica){

    int nro_frame = dir_fisica->nro_frame;
    int desplazamiento = dir_fisica->desplazamiento;

    void* espacio_de_mem = (char*)memoria + (nro_frame * tam_pagina) + desplazamiento;

    char* leido = malloc(tamanio);

    if (tamanio == 1){
        uint8_t leo;
        memcpy(&leo, espacio_de_mem, tamanio);
        leido = int_to_char(leo);
    }
    if(tamanio ==4){
        uint32_t leo;
        memcpy(&leo, espacio_de_mem, tamanio);
        leido = int_to_char(leo);
    }
    if(tamanio== 8){
        memcpy(leido, espacio_de_mem, tamanio);
    }

    //printf("Leo en Nro Frame <%d> y Desp <%d>: <%s>\n", nro_frame, desplazamiento, leido);

    return leido;
}

int frame_sig_disp( uint32_t pid, int frame){
    int frame_siguiente_disp=-1;
    t_tabla* tabla_pid = buscar_por_pid_return(pid);
    bool encontrado = false;
    for (int i = frame; i < bitarray->size; i++) {
        if (bitarray_test_bit(bitarray, i) == 1 && bitarray_test_bit(escrito, i)==0) {
            for(int x = 0; x< list_size(tabla_pid->tabla); x++){
                if(list_get(tabla_pid->tabla,x) == i){
                    frame_siguiente_disp = i;
                    encontrado = true;
                    break;
                }  
                    }
                }
            if (encontrado) {
                break;
            }
    }
    if(frame_siguiente_disp==-1){ // si hay libres pero por arriba del frame
    for (int i = 0; i < frame; i++) {
        if (bitarray_test_bit(bitarray, i) == 1 && bitarray_test_bit(escrito, i)==0) {
            for(int x = 0; x< list_size(tabla_pid->tabla); x++){
                if(list_get(tabla_pid->tabla,x) == i){
                    frame_siguiente_disp = i;
                    encontrado = true;
                    break;
                }  
                    }
                }
            if (encontrado) {
                break;
            }
    }
    }
    return frame_siguiente_disp;
}

bool puede_escribir(uint32_t pid, int frame,int cant_pags ){
    
    t_tabla* tabla_pid = list_get(tabla_pags, pid);
    t_list* tabla_paginas_pid = tabla_pid->tabla;
    
    bool puede = false; 

    bool encontrado = false;

    //FRAME PERTENCE AL PID
    for(int i=0; i< list_size(tabla_paginas_pid); i++){
        int frame_encontrado = list_get(tabla_paginas_pid, i);
        if(frame_encontrado == frame){
            encontrado=true;
        }
    }


    //FRAME ESTA DISPONIBLE EN BIT ARRAY
    bool disponible = false;
    if(bitarray_test_bit(escrito, frame)==0 && bitarray_test_bit(bitarray, frame)==1){
        disponible=true;
    }
    //VER SI TIENE CANT PAGS NECESARIAS
    
    int cont=0;
    for (int i = 0; i < bitarray->size; i++) {
        if (bitarray_test_bit(bitarray, i) == 1 && bitarray_test_bit(escrito, i)==0) {
            for(int x = 0; x< list_size(tabla_pid->tabla); x++){
                if(list_get(tabla_pid->tabla,x) == i){
                   cont++;
                }  
                    }
                }
    }
    if(disponible && encontrado && (cant_pags <= cont)){
        puede = true;
    }

    return puede;
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



