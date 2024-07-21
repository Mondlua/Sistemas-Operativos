#include "memoriautils.h"

int tam_pagina;
int tam_memoria;
t_list* tabla_pags;
void* memoria;
t_bitarray* bitarray;
t_bitarray* escrito;


void escribir_en_mem_io(char* aescribir, t_dir_fisica* dir_fisica, int tamanio, uint32_t pid ){


    string_n_append(&aescribir,"                                                 ", tamanio-strlen(aescribir));

    int nro_frame = dir_fisica->nro_frame;
    int desplazamiento = dir_fisica->desplazamiento;
  
    int cant_pags_necesarias = tamanio/tam_pagina;

    log_info(memoria_log, "cant pags %d \n", cant_pags_necesarias);
    log_info( memoria_log, "desp %d \n", desplazamiento);
    log_info(memoria_log ,"frame %d \n", nro_frame);
    log_info(memoria_log, "a escribir <%s> long %d\n", aescribir, strlen(aescribir));
    log_info(memoria_log, "tamanio <%d> \n", tamanio);

    int resto_desp=tamanio % tam_pagina;
    log_info(memoria_log, "resto %d", resto_desp);
    int resto = 0;
    int cont = 0;
    if(resto_desp != 0){
        cant_pags_necesarias++;
        resto=1;
    }
    log_info(memoria_log, "cant pags %d \n", cant_pags_necesarias);
    int pagina_comienza=0;
    char** arr;
    t_tabla* tabla_pid=buscar_por_pid_return(pid);
    t_list* tabla=tabla_pid->tabla;
    int tam_primera=tam_pagina - desplazamiento;

    if(tam_primera <= strlen(aescribir) && cant_pags_necesarias ==1){
        cant_pags_necesarias++;
    }



    if(puede_escribir(pid,nro_frame, cant_pags_necesarias)){

        if((strlen(aescribir) < tam_primera) && cant_pags_necesarias == 1){

            memcpy((char*)memoria + (nro_frame * tam_pagina) + desplazamiento , aescribir, strlen(aescribir));
            bitarray_set_bit(escrito, nro_frame);  
            printf("2entre\n");
        }
        if(cant_pags_necesarias > 1 ){

        arr=dividir_str_segun_pags(aescribir, cant_pags_necesarias, desplazamiento, resto); 
         printf("primer q leo %s>\n", arr[0]);
        memcpy((char*)memoria + (nro_frame * tam_pagina) + desplazamiento , arr[0], strlen(arr[0]));
        bitarray_set_bit(escrito, nro_frame);  

        for(int i=1; i<cant_pags_necesarias;i++){
            int frame_sig=frame_sig_disp(pid, nro_frame);
            if(i!=cant_pags_necesarias-1){
            memcpy((char*)memoria + (frame_sig * tam_pagina) , arr[i], strlen(arr[i]));
            printf("leo <%s>\n", arr[i]);
            bitarray_set_bit(escrito, frame_sig);
            nro_frame=frame_sig;
           }
           else{
            printf("leo <%s>\n", arr[i]);
            memcpy((char*)memoria + (frame_sig * tam_pagina), arr[i], strlen(arr[i]));
            printf("leo <%s>\n", arr[i]);
            bitarray_set_bit(escrito, frame_sig);
           }
        }
        }
    log_info(memoria_log, "PID: %u - Accion:ESCRIBIR - Direccion fisica: %d - Tamaño %d",pid ,nro_frame+desplazamiento,tamanio);

    }
    else{
        log_error(memoria_log, "No puede escribir");
    }
        
}


void escribir_en_mem_cpu(char* aescribir, t_dir_fisica* dir_fisica, int tamanio , uint32_t pid){
    
    int tam_bytes = tamanio;
    int nro_frame = dir_fisica->nro_frame;
    int desplazamiento = dir_fisica->desplazamiento;
    if(puede_escribir(pid, nro_frame, 1)){

    if (tamanio == 1){ //uint8
        uint8_t escribir = (uint8_t) atoi(aescribir);
        memcpy((char*)memoria + (nro_frame * tam_pagina) + desplazamiento , &escribir, tam_bytes); 
    }
    if(tamanio ==4){//uint32
        uint32_t escribir = (uint32_t)atoi(aescribir);
        memcpy((char*)memoria + (nro_frame * tam_pagina) + desplazamiento , &escribir, tam_bytes);
    }

    log_info(memoria_log, "PID: %u - Accion:ESCRIBIR - Direccion fisica: %d - Tamaño %d",pid ,nro_frame+desplazamiento,tamanio);
    }
    else
    {
        log_error(memoria_log, "No puede escribir"); //escribir bien
    }
}


char** dividir_str_segun_pags(char* str, int cantpags, int desplazamiento, int resto){

    int contador=0;
    int pos=0;
    char** arr=(char**)malloc(sizeof(char*)*(cantpags+resto));
    printf("longitud q me pasan es %d", strlen(str));
    if(resto==0 && desplazamiento == 0){
        log_info(memoria_log,"entre1");
    while(contador<cantpags){
        char* substring= decstring(str,pos,pos+tam_pagina);
        arr[contador]=substring;
        pos = pos + tam_pagina +1;
        contador++;
    }
    }
    if(resto!=0 && desplazamiento == 0){
        log_info(memoria_log,"entre2");
       while(contador<cantpags-1){
        char* substring= decstring(str,pos,pos+tam_pagina);
        arr[contador]=substring;
        pos = pos + tam_pagina +1;
        contador++;
        } 
        char* subs=decstring(str,pos,strlen(str));
        arr[contador]=subs;
    }
    if(desplazamiento != 0 ){
        log_info(memoria_log,"entre3");
        //PRIMER PAG
        int tam_primera = tam_pagina-desplazamiento;
        log_info(memoria_log," tam primera %d", tam_primera);
        char* substring= decstring(str,pos,tam_primera-1); //"hola cc             "
        log_info(memoria_log,"  primera %s", substring);   // 01234567890123456789
        pos = tam_primera; ///PARA QUE SALGA D LA POS
        arr[contador]=substring;
        contador++;
 
        while(contador<cantpags-1){
        char* substring= decstring(str,pos,pos+tam_pagina);
        arr[contador]=substring;
        pos = pos + tam_pagina +1;
        contador++;
        }    
        //ULTIMA PAG
        char* subs=decstring(str,pos,strlen(str)-1);
        printf(" subs es <%s>", subs);
        arr[contador]=subs;
    }

    return arr;
}

//INCLUYE EL END
char* decstring(const char* str, int start, int end) {
    //posicion inicial arranca en 0 

    printf(" long %d", strlen(str));
    printf(" str es %s", str);
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
char* leer_en_mem_cpu(int tamanio, t_dir_fisica* dir_fisica, uint32_t pid){

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
    log_info(memoria_log, "PID: %u - Accion:LEER - Direccion fisica: %d - Tamaño %d",pid ,nro_frame+desplazamiento,tamanio);
    return leido;
}

char* leer_en_mem_io(int tamanio, t_dir_fisica* dir_fisica, uint32_t pid){

    int nro_frame = dir_fisica->nro_frame;
    int desplazamiento = dir_fisica->desplazamiento;
   
    int cant_pags_necesarias = tamanio/tam_pagina;
    int resto_desplazamiento = tamanio % tam_pagina;
    if(resto_desplazamiento != 0){
        cant_pags_necesarias++;
    }

    t_tabla* tabla_pid = buscar_por_pid_return(pid);
    t_list* tabla = tabla_pid->tabla;

	char* leido =malloc(tam_pagina);
    leido= string_new();
    void* espacio_de_mem = (char*)memoria + (nro_frame * tam_pagina) + desplazamiento;
    int tam_primera = tam_pagina - desplazamiento;

    if(tamanio > tam_primera && cant_pags_necesarias == 1){
        cant_pags_necesarias++;
    }

    char* leo=malloc(tam_pagina);
    if(cant_pags_necesarias > 1 || tamanio > tam_primera){

    printf("entre al primero \n");
    memcpy(leo, espacio_de_mem, tam_pagina-desplazamiento);
    string_append(&leido, leo);
    printf("lei <%s>\n", leo);
    
    for (int i = 1; i <cant_pags_necesarias ; i++) {
        if(i== cant_pags_necesarias-1){
        nro_frame = frame_sig_leer(pid, nro_frame);
        void* espacio_de_mem = (char*)memoria + (nro_frame * tam_pagina);
        char* leeo=malloc(tam_pagina);;
        int tam_ultimo=tamanio-(tam_pagina-desplazamiento)-(cant_pags_necesarias-2)*tam_pagina;
        printf("tam <%d>\n", tamanio);
        printf("tam ultimo <%d>\n", tam_ultimo);
        leeo[tam_ultimo] = '\0';
        memcpy(leeo, espacio_de_mem , tam_ultimo);
        
        printf("lei <%s>\n", leeo);
        string_append(&leido,leeo);
        printf("leido va quedando <%s>\n", leido);
       }
       else{
        int frame = frame_sig_leer(pid, nro_frame);
        void* espacio_de_mem = (char*)memoria + (frame * tam_pagina);
        nro_frame = frame;
        char* leeo=malloc(tam_pagina);;
        memcpy(leeo, espacio_de_mem, tam_pagina);

        printf("lei %s>\n", leeo);
        string_append(&leido,leeo);
       }
    }
    }
    else{
        printf("entre aca\n");
        leo[tamanio] = '\0';
        memcpy(leo, espacio_de_mem, tamanio);
        printf("lei <%s>\n", leo);
        string_append(&leido,leo);
      
    }
    log_info(memoria_log, "PID: %u - Accion:LEER - Direccion fisica: %d - Tamaño %d",pid ,nro_frame+desplazamiento,tamanio);
    
    string_trim_right(&leido);
    printf("el leido completo <%s>\n", leido);
    return leido;
}

int frame_sig_disp( uint32_t pid, int frame){
    int frame_siguiente_disp=-1;
    t_tabla* tabla_pid = buscar_por_pid_return(pid);
    bool encontrado = false;
    for (int i = frame+1; i < bitarray->size; i++) {
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

int frame_sig_leer( uint32_t pid, int frame){
    int frame_siguiente=-1;
    t_tabla* tabla_pid = buscar_por_pid_return(pid);
    bool encontrado = false;
    for (int i = frame+1; i < bitarray->size; i++) {
        if (bitarray_test_bit(bitarray, i) == 1 && bitarray_test_bit(escrito, i)==1) {
            for(int x = 0; x< list_size(tabla_pid->tabla); x++){
                if(list_get(tabla_pid->tabla,x) == i){
                    frame_siguiente = i;
                    encontrado = true;
                    break;
                }  
                    }
                }
            if (encontrado) {
                break;
            }
    }
    if(frame_siguiente==-1){ // si hay libres pero por arriba del frame
    for (int i = 0; i < frame; i++) {
        if (bitarray_test_bit(bitarray, i) == 1 && bitarray_test_bit(escrito, i)==1) {
            for(int x = 0; x< list_size(tabla_pid->tabla); x++){
                if(list_get(tabla_pid->tabla,x) == i){
                    frame_siguiente = i;
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
    return frame_siguiente;
}

bool puede_escribir(uint32_t pid, int frame,int cant_pags ){
    
    t_tabla* tabla_pid = buscar_por_pid_return(pid);
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
    if( bitarray_test_bit(bitarray, frame)==1){//bitarray_test_bit(escrito, frame)==0 &&
        disponible=true;
    }
    //VER SI TIENE CANT PAGS NECESARIAS
    int cont=0;
    if(cant_pags != 1){
    
    for (int i = 0; i < bitarray->size; i++) {
        if (bitarray_test_bit(bitarray, i) == 1 && bitarray_test_bit(escrito, i)==0) {
            for(int x = 0; x< list_size(tabla_pid->tabla); x++){
                if(list_get(tabla_pid->tabla,x) == i){
                   cont++;
                }  
                    }
                }
    }
    }
    if(cant_pags == 1){
        cont = cant_pags;
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



