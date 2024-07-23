#include "DialFS.h"

t_list* lista_archivos;
t_bitarray* bitmap;
size_t bitmap_size;
char bitmap_path[256];
char blocks_path[256];

void inicio_filesystem() {

    /* Archivo de Bloques */
    
    snprintf(blocks_path, sizeof(blocks_path), "%s/bloques.dat", path_base_dialfs);
    int blocks_file = open(blocks_path, O_CREAT | O_RDWR, 0666);
    if (blocks_file < 0) {
        perror("Error al crear bloques.dat");
        exit(EXIT_FAILURE);
    }

    off_t tamanio_archivo = (off_t) block_size * block_size;
    if (ftruncate(blocks_file, tamanio_archivo) < 0) {
        perror("No se pudo establecer el tamanio del archivo");
        close(blocks_file);
        exit(EXIT_FAILURE);
    }
    close(blocks_file);

    /* Archivo de Bitmap */
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

    lista_archivos = list_create();
}


void crear_archivo(char* nombre){
    usleep(tiempo_unidad_trabajo * 1000);

    char new_file_path[256];
    snprintf(new_file_path, sizeof(new_file_path), "%s/%s", path_base_dialfs, nombre);
    int fd = open(new_file_path, O_CREAT | O_RDWR, 0666);
    close(fd);
    
    t_config* new_file_config;
    new_file_config = config_create(new_file_path);
    config_set_value(new_file_config, "TAMANIO_ARCHIVO", "0");
    
    uint32_t bloque_libre = buscar_bloque_libre();
    if (bloque_libre == -1) {
        printf("No hay bloques libres disponibles.\n");
        munmap(bitmap->bitarray, bitmap_size);
        exit(EXIT_FAILURE);
    }
    bitarray_set_bit(bitmap, bloque_libre);
    msync(bitmap->bitarray, bitmap_size, MS_SYNC); // Sincroniza los cambios del bitmap

    config_set_value(new_file_config, "BLOQUE_INICIAL", int_to_char(bloque_libre));
    config_save(new_file_config);
    config_destroy(new_file_config);

    Archivo* file = malloc(sizeof(Archivo));
    file->nombre = nombre;
    file->ruta = new_file_path;
    file->comienzo = bloque_libre;
    file->tamanio = 0;
    list_add(lista_archivos, file);
}

void borrar_archivo(char* nombre){
    usleep(tiempo_unidad_trabajo * 1000);

    Archivo* archivo = buscar_archivo_por_nombre(nombre);
    t_config* file_config = config_create(archivo->ruta);
    
    int bloque_inicial = config_get_int_value(file_config, "BLOQUE_INICIAL");
    int tamanio_archivo = config_get_int_value(file_config, "TAMANIO_ARCHIVO");

    int cantidad_bloques = (tamanio_archivo + block_size - 1) / block_size; //Es para redondear hacia arriba

    for (int contador_bloques = bloque_inicial; contador_bloques < bloque_inicial + cantidad_bloques; contador_bloques++) {
        bitarray_clean_bit(bitmap, contador_bloques);
    }

    config_destroy(file_config);
    if (unlink(archivo->ruta) == 0) {
        printf("Archivo eliminado exitosamente.\n");
    } else {
        perror("Error al eliminar el archivo");
    }
    FILE* archivo_bloques = fopen(blocks_path, "rb+");
    char* buffer = calloc(block_size, 1); //lleno de 0
    fseek(archivo_bloques, bloque_inicial * block_size, SEEK_SET);
    for (int i = 0; i < cantidad_bloques; i++) {
        fwrite(buffer, block_size, 1, archivo_bloques);
    }
    fclose(archivo_bloques);
    free(buffer);
    
    int bitmap_file = open(bitmap_path, O_RDWR);
    msync(bitmap->bitarray, bitmap_size, MS_SYNC);
    close(bitmap_file);
    eliminar_archivo_de_lista(bloque_inicial);
}

void truncar_archivo(char* nombre, uint32_t tamanio, uint32_t pid){
    usleep(tiempo_unidad_trabajo * 1000);

    Archivo* archivo = buscar_archivo_por_nombre(nombre);
    t_config* file_config = config_create(archivo->ruta);
    
    int bloque_inicial = config_get_int_value(file_config, "BLOQUE_INICIAL");
    int cantidad_bloques = (tamanio + block_size - 1) / block_size; //Es para redondear hacia arriba
    int bloque_final = bloque_inicial + cantidad_bloques - 1;

    int tamanio_anterior = config_get_int_value(file_config, "TAMANIO_ARCHIVO");
    int bloque_final_anterior;
    if(tamanio_anterior == 0) {
        bloque_final_anterior = bloque_inicial;
    }
    else{
        int bloques_ocupados_anteriormente = (tamanio_anterior + block_size - 1) / block_size;
        bloque_final_anterior = bloque_inicial + bloques_ocupados_anteriormente - 1;
    }
    
    if (bloque_final_anterior > bloque_final) { //Achicar
        for (int i = bloque_final + 1; i <= bloque_final_anterior; i++) {
            bitarray_clean_bit(bitmap, i);
        }
    }
    else if (bloque_final != bloque_final_anterior) { //Agrandar
        if (!bloques_contiguos_libres(bloque_final_anterior, bloque_final)){
            log_info(entradasalida_log, "PID: <%i> - Inicio Compactación.", pid);
            compactar(&bloque_inicial, bloque_final);
            log_info(entradasalida_log, "PID: <%i> - Fin Compactación.", pid);
            bloque_final = bloque_inicial + cantidad_bloques - 1;
        }
        for (int i = bloque_inicial; i <= bloque_final; i++) {
            bitarray_set_bit(bitmap, i);
        }
    }

    config_set_value(file_config, "TAMANIO_ARCHIVO", int_to_char(tamanio));
    config_save(file_config);
    archivo->tamanio = tamanio;
    config_destroy(file_config);
    int bitmap_file = open(bitmap_path, O_RDWR);
    msync(bitmap->bitarray, bitmap_size, MS_SYNC);
    close(bitmap_file);
}

void escribir_archivo(char* nombre, off_t puntero, char* a_escribir, uint32_t tamanio){
    usleep(tiempo_unidad_trabajo * 1000);

    Archivo* archivo = buscar_archivo_por_nombre(nombre);
    FILE* archivo_bloques = fopen(blocks_path, "rb+");
    fseeko(archivo_bloques, (archivo->comienzo * block_size) + puntero, SEEK_SET);
    fwrite(a_escribir, tamanio, 1, archivo_bloques);
    fclose(archivo_bloques);
}

char* leer_archivo(char* nombre, off_t puntero, uint32_t tamanio){
    usleep(tiempo_unidad_trabajo * 1000);
    Archivo* archivo = buscar_archivo_por_nombre(nombre);
    FILE* archivo_bloques = fopen(blocks_path, "rb");
    fseeko(archivo_bloques, (archivo->comienzo * block_size) + puntero, SEEK_SET);
    char* buffer = malloc(tamanio + 1);
    fread(buffer, 1, tamanio, archivo_bloques);
    buffer[tamanio] = '\0';
    fclose(archivo_bloques);

    return buffer;
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

Archivo* buscar_archivo_por_nombre(char* nombre) {
    Archivo* archivo = malloc(sizeof(Archivo));
    for (int i = 0; i < list_size(lista_archivos); i++) {
        archivo = (Archivo*)list_get(lista_archivos, i);
        if (strcmp(archivo->nombre, nombre) == 0) {
            return archivo;
        }
    }
    return NULL;
}

int buscar_archivo_x_bloque_inicial(int bloque_inicial){
    for (int i = 0; i < list_size(lista_archivos); i++) {
        Archivo* file = (Archivo*)list_get(lista_archivos, i);
        if (file->comienzo == bloque_inicial) {
            return i; 
        }
    }
    return -1; 
}


void eliminar_archivo_de_lista(int bloque_inicial){
    int posicion_a_borrar=  buscar_archivo_x_bloque_inicial(bloque_inicial);

    if (posicion_a_borrar != -1) {
        list_remove_and_destroy_element(lista_archivos, posicion_a_borrar, destruir_file);
    } else {
        printf("Archivo no encontrado en la lista.\n");
    }
}

void destruir_file(void* elemento){
    Archivo* file = (Archivo*)elemento;
    free(file->ruta);
    free(file);
}

bool bloques_contiguos_libres(int bloque_inicial, int bloque_final){
    for (int i = bloque_inicial + 1; i <= bloque_final; i++) {
        if (leer_de_bitmap(i) != 0) {
            return false;
        }
    }
    return true;
}

bool comparar_archivos_por_bloque_inicial(void* a, void* b) {
    Archivo* archivo_a = *(Archivo**)a;
    Archivo* archivo_b = *(Archivo**)b;
    return archivo_a->comienzo - archivo_b->comienzo;
}



void escribir_datos_en_nuevos_bloques(int* bloques_originales, char** buffers_datos, int num_bloques, int bloque_libre_actual) {
    FILE* archivo_bloques = fopen(blocks_path, "rb+");
    for (int i = 0; i < num_bloques; i++) {
        Archivo* archivo = (Archivo*)list_get(lista_archivos, buscar_archivo_x_bloque_inicial(bloques_originales[i]));
        int bloque_final_archivo = archivo->comienzo + ((archivo->tamanio + block_size - 1) / block_size) - 1;

        fseek(archivo_bloques, bloque_libre_actual * block_size, SEEK_SET);
        fwrite(buffers_datos[i], archivo->tamanio, 1, archivo_bloques);

        for (int bloque = bloques_originales[i]; bloque <= bloque_final_archivo; bloque++) {
            bitarray_clean_bit(bitmap, bloque);
            bitarray_set_bit(bitmap, bloque_libre_actual);
            bloque_libre_actual++;
        }
        archivo->comienzo = bloque_libre_actual;
        t_config* new_file_config = config_create(archivo->ruta);
        config_set_value(new_file_config, "BLOQUE_INICIAL", int_to_char(archivo->comienzo));
        config_destroy(new_file_config);
    }
    fclose(archivo_bloques);
}

int limpiar_bloques_y_buscar_libres(int bloque_inicial, int bloque_final) {
    for (int i = bloque_inicial; i <= bloque_final; i++) {
        bitarray_clean_bit(bitmap, i);
    }
    int siguiente_bloque = 0;
    while (leer_de_bitmap(siguiente_bloque) != 0) {
        siguiente_bloque++;
    }
    return siguiente_bloque;
}

char** guardar_contenido_bloques(int* bloques_originales, int num_bloques) {
    FILE* archivo_bloques = fopen(blocks_path, "rb+");
    char** buffers_datos = malloc(sizeof(char*) * num_bloques);
    for (int i = 0; i < num_bloques; i++) {
        buffers_datos[i] = malloc(block_size);
        fseek(archivo_bloques, bloques_originales[i] * block_size, SEEK_SET);
        fread(buffers_datos[i], block_size, 1, archivo_bloques);
    }
    fclose(archivo_bloques);
    return buffers_datos;
}

void compactar(int* bloque_inicial, int bloque_final) {
    list_sort(lista_archivos, comparar_archivos_por_bloque_inicial);

    int num_bloques = 0;
    int* bloques_originales = malloc(sizeof(int) * list_size(lista_archivos));
    char** buffers_datos = NULL;
    for (int i = 0; i < list_size(lista_archivos); i++) {
        Archivo* archivo = (Archivo*)list_get(lista_archivos, i);
        int bloque_final_archivo = archivo->comienzo + ((archivo->tamanio + block_size - 1) / block_size) - 1;
        if (archivo->comienzo > bloque_final) {
            bloques_originales[num_bloques] = archivo->comienzo;
            num_bloques++;
        }
    }
    buffers_datos = guardar_contenido_bloques(bloques_originales, num_bloques);
    if (buffers_datos == NULL) {
        free(bloques_originales);
        return;
    }
   
    int bloque_libre_actual = limpiar_bloques_y_buscar_libres(*bloque_inicial, bloque_final);

    escribir_datos_en_nuevos_bloques(bloques_originales, buffers_datos, num_bloques, bloque_libre_actual);
    free(bloques_originales);
    for (int i = 0; i < num_bloques; i++) {
        free(buffers_datos[i]);
    }
    free(buffers_datos);
    int bitmap_file = open(bitmap_path, O_RDWR);
    msync(bitmap->bitarray, bitmap_size, MS_SYNC);
    close(bitmap_file);

    usleep(retraso_compactacion * 1000);
}
