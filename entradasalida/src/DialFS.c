#include "DialFS.h"

t_bitarray* bitmap;
size_t bitmap_size;

void inicio_filesystem() {

    /* Archivo de Bloques */
    char blocks_path[256];
    snprintf(blocks_path, sizeof(blocks_path), "%s/bloques.dat", path_base_dialfs);
    int blocks_file = open(blocks_path, O_CREAT | O_RDWR, 0666);
    if (blocks_file < 0) {
        perror("Error al crear bloques.dat");
        exit(EXIT_FAILURE);
    }

    off_t tamaño_archivo = (off_t) block_size * block_size;
    if (ftruncate(blocks_file, tamaño_archivo) < 0) {
        perror("No se pudo establecer el tamaño del archivo");
        close(blocks_file);
        exit(EXIT_FAILURE);
    }
    close(blocks_file);

    /* Archivo de Bitmap */
    char bitmap_path[256];
    snprintf(bitmap_path, sizeof(bitmap_path), "%s/bitmap.dat", path_base_dialfs);
    struct stat st;
    int bitmap_file;

    if (stat(bitmap_path, &st) == 0) { // Si el archivo existe
        bitmap_file = open(bitmap_path, O_RDWR);
        if (bitmap_file < 0) {
            perror("Error al abrir bitmap.dat");
            exit(EXIT_FAILURE);
        }
        bitmap_size = st.st_size;
    } else { // Si el archivo no existe, crea el bitmap inicial, crea el archivo y lo guarda
        bitmap_file = open(bitmap_path, O_CREAT | O_RDWR, 0666);
        if (bitmap_file < 0) {
            perror("Error al abrir bitmap.dat");
            exit(EXIT_FAILURE);
        }

        bitmap_size = (block_count + 7) / 8; // Redondea hacia arriba los bytes
        char* bitmap_data = (char *)calloc(bitmap_size, sizeof(char)); // sizeof de un char es 1 byte
        bitmap = bitarray_create_with_mode(bitmap_data, bitmap_size, LSB_FIRST);

        ftruncate(bitmap_file, bitmap_size);

        char* bitmap_mmap = mmap(NULL, bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, bitmap_file, 0);
        if (bitmap_mmap == MAP_FAILED) {
            perror("Error al mapear bitmap.dat");
            free(bitmap_data);
            bitarray_destroy(bitmap);
            close(bitmap_file);
            exit(EXIT_FAILURE);
        }

        memcpy(bitmap_mmap, bitmap_data, bitmap_size);
        munmap(bitmap_mmap, bitmap_size);
        free(bitmap_data);
    }

    // Mapear el archivo bitmap a memoria
    char* bitmap_mmap = mmap(NULL, bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, bitmap_file, 0);
    if (bitmap_mmap == MAP_FAILED) {
        perror("Error al mapear bitmap.dat 2.0");
        close(bitmap_file);
        exit(EXIT_FAILURE);
    }
    bitmap = bitarray_create_with_mode(bitmap_mmap, bitmap_size, LSB_FIRST);
    close(bitmap_file); 
}


void crear_archivo(char* nombre){
    usleep(tiempo_unidad_trabajo);

    char new_file_path[256];
    snprintf(new_file_path, sizeof(new_file_path), "%s/%s", path_base_dialfs, nombre);
    int new_file = open(new_file_path, O_CREAT | O_RDWR, 0666);
    if (new_file < 0) {
        perror("Error al crear el archivo");
        exit(EXIT_FAILURE);
    }
    ftruncate(new_file, 0);

    uint32_t bloque_libre = buscar_bloque_libre();
    if (bloque_libre == -1) {
        printf("No hay bloques libres disponibles.\n");
        munmap(bitmap->bitarray, bitmap_size);
        close(new_file);
        exit(EXIT_FAILURE);
    }
    bitarray_set_bit(bitmap, bloque_libre);
    msync(bitmap->bitarray, bitmap_size, MS_SYNC); // Sincroniza los cambios del bitmap
    
    t_config* new_file_config;
    new_file_config = config_create(new_file_path);
    config_set_value(new_file_config, "TAMANIO_ARCHIVO", "0");
    config_set_value(new_file_config, "BLOQUE_INICIAL", int_to_char(bloque_libre));
    config_save(new_file_config);
    config_destroy(new_file_config);
}

void borrar_archivo(char* nombre){
    
}


uint32_t buscar_bloque_libre(){
    for(int nroBloque = 0; nroBloque < block_count; nroBloque++){
        if (!leer_de_bitmap(nroBloque)) {
            return nroBloque;
        }
    }
    return -1;
}

uint8_t leer_de_bitmap(uint32_t nroBloque){
    return bitarray_test_bit(bitmap, nroBloque);
}