#include "instrucciones_memoria.h"


t_list* abrir_pseudocodigo(char* path){

    char* path_instrucciones = config_get_string_value(memoria_config ,"PATH_INSTRUCCIONES");
    size_t total_length = strlen(path_instrucciones) + strlen(path) + 2; // 1 para el '/' y 1 para el '\0'
    char* path_completo = malloc(total_length);
    snprintf(path_completo, total_length, "%s%s", path_instrucciones, path);
    //snprintf(path_completo, total_length, "%s/%s", path_instrucciones, path);
   /* if (path_instrucciones[strlen(path_instrucciones) - 1] == '/') {
        snprintf(path_completo, total_length, "%s%s", path_instrucciones, path);
    } else {
        snprintf(path_completo, total_length, "%s/%s", path_instrucciones, path);
    }*/
    log_info(memoria_log, path_completo);
    FILE* arch_pseudocodigo = fopen(path_completo, "r");
        if(arch_pseudocodigo == NULL){
            log_error(memoria_log, "No se pudo abrir el archivo.\n");
            EXIT_FAILURE;
        }
    free(path_completo);

    //FILE* arch_pseudocodigo = fopen(strcat(strcat(path_instrucciones,"/"),path), "r"); // strcat no permite crear mas de 1 proceso

    t_list* lista_inst= list_create();
    char instruccionlinea[50]; //Ver 

    int cont = 0;
        while (fgets(instruccionlinea, sizeof(instruccionlinea), arch_pseudocodigo) != NULL) {
            
            size_t longitud = strlen(instruccionlinea);
            t_instruccion* instruccion = malloc(sizeof(t_instruccion));
            instruccion->buffer=malloc(sizeof(t_buffer_ins));
            instruccion->codigo_operacion = INSTRUCCION;
            instruccion->buffer->size = longitud;
            instruccion->buffer->stream = malloc(sizeof(instruccionlinea));
            instruccion->buffer->stream = strdup(instruccionlinea);
            list_add_in_index(lista_inst,cont,instruccion);
            cont++;
            printf("Longitud: %zu, Contenido: %s", instruccion->buffer->size, instruccion->buffer->stream);
            sem_post(&semaforo_mem); //Contador de intrucciones
            if(strcmp(instruccion->buffer->stream,"EXIT") == 0){
                 break;  
            }
        }   
    

    log_info(memoria_log,"Cerrando archivo pseudocodigo...");
    fclose(arch_pseudocodigo);
    return lista_inst;
}
