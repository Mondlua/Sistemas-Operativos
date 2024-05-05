#include "instrucciones_memoria.h"


typedef struct{
    char* instruccion;
    uint32_t tam_instruccion;
}t_instruccion;


// Abrir archivo
FILE* arch_pseudocodigo;
arch_pseudocodigo = fopen("prueba_instrucciones.txt", "r");  // en este fopen tendria que recibir PATH_INSTRUCCIONES + "PATH" (recibido de INICIAR_PROCESO)
    if(arch_pseudocodigo == NULL){
        log_error(memoria_log, "No se pudo abrir el archivo.\n");
        EXIT_FAILURE;
    }

uint32_t instruccion_size = 2; // contando el caracter nulo y '\n' ; El caracter nulo (\0 me lo agrega la funcion fgets())
char caracter;

caracter = fgetc(arch_pseudocodigo);
while(caracter!=EOF) {

    while(caracter != '\n' && caracter!=EOF){
        instruccion_size++;
        caracter = fgetc(arch_pseudocodigo);
    }

    if(caracter == EOF){
        instruccion_size -=1; // Ya que en la ultima linea del archivo NO hay un \n
        fseek(arch_pseudocodigo, 1, SEEK_CUR);
    }
    // Volver al inicio de la línea para leer la instrucción completa
    fseek(arch_pseudocodigo, -instruccion_size, SEEK_CUR);

    t_instruccion* instruccion = malloc(sizeof(t_instruccion));
    //instruccion->tam_instruccion = malloc(sizeof(int));
    instruccion->tam_instruccion = instruccion_size;
    instruccion->instruccion = malloc(instruccion->tam_instruccion);

    fgets(instruccion->instruccion, instruccion->tam_instruccion, arch_pseudocodigo);
    //list_add(instrucciones_memoria, instruccion);
    if(strcmp(instruccion->instruccion,"EXIT") == 0){
       break;  
    }

   // free(instruccion->tam_instruccion);
    free(instruccion->instruccion);
    free(instruccion);

    caracter = fgetc(arch_pseudocodigo); // Leer el salto de línea o EOF
    instruccion_size = 2; // Reiniciar el contador de tamaño de instrucción
}
printf("Cerrando archivo...");
fclose(arch_pseudocodigo);

//--------------------------------------------------------------------------------------
// CPU
enviar_pc(conexion_memoria, pcb->p_counter){
    send(conexion_memoria, PROGRAM_COUNTER, sizeof(int), MSG_NOSIGNAL);
    send(conexion_memoria, pcb->p_counter, sizeof(uint32_t), MSG_NOSIGNAL);
}

recibir_instruccion(socket_cliente){
     // FALTA IMPLEMENTAR
}

//--------------------------------------------------------------------------------------
// MEMORIA
uint32_t recibir_program_counter(conexion){
    uint32_t program_counter;
    recv(conexion, program_counter, sizeof(uint32_t), MSG_WAITALL);
    return program_counter;
}

uint32_t pc = recibir_program_counter(conexion);
t_instruccion* instruccion_a_enviar;
instruccion_a_enviar = list_get(instrucciones_memoria, pc);
enviar_instruccion(socket_cpu_dispatch, instruccion_a_enviar);

//---------ENVIAR INSTRUCCION------------------------

enviar_instruccion(socket_cliente, instruccion){
t_paquete* paquete = malloc(sizeof(t_paquete));
    int offset=0;

	paquete->codigo_operacion = INSTRUCCION;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = sizeof(uint32_t) + instruccion.tam_instruccion;
	paquete->buffer->stream = malloc(paquete->buffer->size);

    memcpy(paquete->buffer->stream + offset, instruccion->tam_instruccion, sizeof(uint32_t))
    offset += sizeof(uint32_t);
	memcpy(paquete->buffer->stream + offset, instruccion->instruccion, instruccion.tam_instruccion);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

// Luego la CPU, con strtok() podria leer la hasta que haya un " " y ahi tomaria el nombre de la funcion (p.e. SET)
// en las commons hay funciones de DICCIONARIO, creo que se puede asociar las KEYs(SET, SUM, SUB, etc) con sus respectivas funciones
// para ejecutar las instrucciones segun se corresponda.