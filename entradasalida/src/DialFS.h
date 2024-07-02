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
#include <commons/bitarray.h>
#include <commons/config.h>
#include <utils/funcionesUtiles.h>

extern int block_size;
extern int block_count;
extern int tiempo_unidad_trabajo;
extern char* path_base_dialfs;

void inicio_filesystem();
void crear_archivo(char* nombre);
void borrar_archivo(char* nombre);
void truncar_archivo(char* nombre, uint32_t tamaño);
uint32_t buscar_bloque_libre();
uint8_t leer_de_bitmap(uint32_t nroBloque);

#endif