#include "DialFS.h"

void inicio_filesystem(char* path_base_dialfs){

    /* Archivo de Bloques */

    char blocks_path[256];
    snprintf(blocks_path, sizeof(blocks_path), "%s/bloques.dat", path_base_dialfs);
    int blocks_file = open(blocks_path, O_CREAT | O_RDWR, 0666);
    if (blocks_file <0 ) {
        perror("Error al crear bloques.dat");
        exit(EXIT_FAILURE);
    }

    off_t tamaño_archivo = (off_t) block_size * block_size;
    if(ftruncate(blocks_file, tamaño_archivo) < 0){
        perror("No se pudo establecer el tamaño del archivo");
        close(blocks_file);
        exit(EXIT_FAILURE);
    }
    close(blocks_file);


    /* Archivo de Bitmap */

    char bitmap_path[256];
    snprintf(bitmap_path, sizeof(bitmap_path), "%s/bitmap.dat", path_base_dialfs);
    struct stat st;
    if (stat(bitmap_path, &st) == 0) { //Si el archivo existe
        FILE* bitmap_file = fopen(bitmap_path, "rb+");
        if (bitmap_file == NULL) {
            perror("Error al abrir bitmap.dat");
            exit(EXIT_FAILURE);
        }
        fclose(bitmap_file);
    } else { //Si el archivo no existe, crea el bitmap inicial, crea el archivo y lo guarda
        size_t bitmap_size = (block_count + 7) / 8;
        //size_t bitmap_size = block_count;
        char* bitmap_data = (char *)calloc(bitmap_size, sizeof(char));
        t_bitarray* bitmap = bitarray_create_with_mode(bitmap_data, bitmap_size, LSB_FIRST);

        FILE* bitmap_file = fopen(bitmap_path, "wb");
        if (bitmap_file == NULL) {
            perror("Error al crear bitmap.dat");
            exit(EXIT_FAILURE);
        }

        fwrite(bitmap_data, sizeof(char), bitmap_size, bitmap_file);
        fclose(bitmap_file);
        bitarray_destroy(bitmap);
        free(bitmap_data);
    }
}
