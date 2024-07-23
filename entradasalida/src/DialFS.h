#ifndef DIALFS_H_
#define DIALFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <commons/config.h>
#include <utils/funcionesUtiles.h>
#include <utils/io_operation.h>

typedef struct
{
    char* nombre;
    char* ruta;
    int tamanio;
    int comienzo; //Bloque inicial
}Archivo;


extern t_log* entradasalida_log;
extern int block_size;
extern int block_count;
extern int tiempo_unidad_trabajo;
extern int retraso_compactacion;
extern char* path_base_dialfs;

void inicio_filesystem();
void crear_archivo(char* nombre);
void borrar_archivo(char* nombre);
void truncar_archivo(char* nombre, uint32_t tamanio, uint32_t pid);
void escribir_archivo(char* nombre, off_t puntero, char* a_escribir, uint32_t tamanio);
char* leer_archivo(char* nombre, off_t puntero, uint32_t tamanio);

uint32_t buscar_bloque_libre();
uint8_t leer_de_bitmap(uint32_t nroBloque);
Archivo* buscar_archivo_por_nombre(char* nombre);
int buscar_archivo_x_bloque_inicial(int bloque_inicial);
void eliminar_archivo_de_lista(int bloque_inicial);
void destruir_file(void* elemento);
bool bloques_contiguos_libres(int bloque_inicial, int bloque_final);
void ordenar_archivos_por_comienzo();
int comparar_por_comienzo(const void* a, const void* b);
char** guardar_contenido_bloques(int* bloques_originales, int num_bloques, FILE* archivo_bloques);
int limpiar_bloques_y_buscar_libres(int bloque_inicial, int bloque_final);
void escribir_datos_en_nuevos_bloques(int* bloques_originales, char** buffers_datos, int num_bloques, int* bloque_libre_actual, FILE* archivo_bloques);

void compactar(int* bloque_inicial, int bloque_final, char* nombre);
void guardar_lista_archivos();
void cargar_lista_archivos();

#endif