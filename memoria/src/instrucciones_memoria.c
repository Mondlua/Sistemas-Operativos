#include "instrucciones_memoria.h"


t_list* abrir_pseudocodigo(char* path){

    char* path_instrucciones = config_get_string_value(memoria_config ,"PATH_INSTRUCCIONES");
    FILE* arch_pseudocodigo = fopen(strcat(strcat(path_instrucciones,"/"),path), "r");
        if(arch_pseudocodigo == NULL){
         log_error(memoria_log, "No se pudo abrir el archivo.\n");
         EXIT_FAILURE;
         }

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
            //printf("Longitud: %zu, Contenido: %s", instruccion->buffer->size, instruccion->buffer->stream);
            sem_post(&semaforo_mem); //Contador de intrucciones
            if(strcmp(instruccion->buffer->stream,"EXIT") == 0){
                 break;  
            }
        }   
    

    log_info(memoria_log,"Cerrando archivo pseudocodigo...");
    fclose(arch_pseudocodigo);
    return lista_inst;
}


// Luego la CPU, con strtok() podria leer la hasta que haya un " " y ahi tomaria el nombre de la funcion (p.e. SET)
// en las commons hay funciones de DICCIONARIO, creo que se puede asociar las KEYs(SET, SUM, SUB, etc) con sus respectivas funciones
// para ejecutar las instrucciones segun se corresponda.