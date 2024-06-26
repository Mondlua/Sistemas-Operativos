#include "DialFS.h"

void inicio_filesystem(){

    int tamaño_archivo = block_size * block_size;
    int blocks_file = open("bloques.dat", O_CREAT | O_RDWR, 0666);
    if (blocks_file <0 ) {
        perror("Error al crear bloques.dat");
        exit(EXIT_FAILURE);
    }
    if(ftruncate(blocks_file, tamaño_archivo) < 0){
        perror("No se pudo establecer el tamaño del archivo");
        close(blocks_file);
        exit(EXIT_FAILURE);
    }
    close(blocks_file);

    FILE* bitmap_file = fopen("bitmap.dat", "wb");
    if (bitmap_file == NULL) {
        perror("Error al crear bitmap.dat");
        exit(EXIT_FAILURE);
    }

}