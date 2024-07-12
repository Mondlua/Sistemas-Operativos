#ifndef DIALFS_H_
#define DIALFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <commons/config.h>
#include <utils/funcionesUtiles.h>

typedef struct
{
    char* ruta;
    int tamaño;
    int comienzo;
}Archivo;


extern int block_size;
extern int block_count;
extern int tiempo_unidad_trabajo;
extern int retraso_compactacion;
extern char* path_base_dialfs;

void inicio_filesystem();
void crear_archivo(char* nombre);
void borrar_archivo(char* nombre);
void truncar_archivo(char* nombre, uint32_t tamaño);
uint32_t buscar_bloque_libre();
uint8_t leer_de_bitmap(uint32_t nroBloque);
int buscar_archivo_x_bloque_inicial(int bloque_inicial);
void eliminar_archivo_de_lista(int bloque_inicial);
void destruir_file(void* elemento);
bool bloques_contiguos_libres(int bloque_inicial, int bloque_final);
void encontrar_proximo(void *data);
Archivo* buscar_bloque_inicial_proximo(int bloque_inicial);
void compactar(int*,int*);


#endif