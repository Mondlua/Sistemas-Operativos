#include "main.h"

char* nombre_interfaz;
int conexion_kernel;
int conexion_memoria;
int tiempo_unidad_trabajo;
int retraso_compactacion;
int block_size;
int block_count;
char* path_base_dialfs;
t_log* entradasalida_log;
t_log* log_aux;
t_config* entradasalida_config;  

int main(int argc, char *argv[]) { 

    if (argc < 3) {
        fprintf(stderr, "Uso: %s <nombre_interfaz> <path_config>\n", argv[0]);
        return 1;
    }

    char* ip_kernel; 
    char* puerto_kernel;
    
    char* interfaz;
    char* ruta;

    entradasalida_log = iniciar_logger("entradasalida.log","entradasalida");
    log_aux = iniciar_logger("log_aux.log", "IO_aux");
    nombre_interfaz = strdup(argv[1]);
    if (nombre_interfaz == NULL) {
        fprintf(stderr, "Error al duplicar nombre_interfaz\n");
        return 1;
    }
    ruta = argv[2];

    log_debug(log_aux,"Nombre interfaz: %s", nombre_interfaz);
    log_debug(log_aux,"Path config: %s", ruta);
    
    entradasalida_config = iniciar_config(ruta);
     if (entradasalida_config == NULL) {
        log_error(log_aux, "Error al iniciar la configuración");
        log_destroy(entradasalida_log);
        log_destroy(log_aux);
        free(nombre_interfaz);
        return 1;
    }
	/* I/O - Cliente */

    interfaz = config_get_string_value(entradasalida_config, "TIPO_INTERFAZ");
    if (interfaz == NULL) {
        log_error(log_aux, "Error al obtener TIPO_INTERFAZ del config");
        config_destroy(entradasalida_config);
        log_destroy(entradasalida_log);
        log_destroy(log_aux);
        free(nombre_interfaz);
        return 1;
    }
    ip_kernel= config_get_string_value(entradasalida_config,"IP_KERNEL");
    puerto_kernel = config_get_string_value(entradasalida_config, "PUERTO_KERNEL");
    if (ip_kernel == NULL || puerto_kernel == NULL) {
        log_error(log_aux, "Error al obtener IP_KERNEL o PUERTO_KERNEL del config");
        config_destroy(entradasalida_config);
        log_destroy(entradasalida_log);
        log_destroy(log_aux);
        free(nombre_interfaz);
        return 1;
    }
    extraer_segun_tipo_io(interfaz);
    
    conexion_kernel = crear_conexion(ip_kernel, puerto_kernel);
    if (conexion_kernel < 0) {
        log_error(log_aux, "Error al conectar con KERNEL");
        config_destroy(entradasalida_config);
        log_destroy(entradasalida_log);
        log_destroy(log_aux);
        free(nombre_interfaz);
        return 1;
    }

    log_debug(log_aux, "I/O conectado a KERNEL");
    send_handshake(conexion_kernel, log_aux, "I/O / KERNEL");
    
    aviso_segun_cod_op(nombre_interfaz, conexion_kernel, INTERFAZ);
    pthread_t hilo;
    if (pthread_create(&hilo, NULL, (void *)recibir_instruccion, interfaz) != 0) {
        log_error(log_aux, "Error al crear el hilo");
        config_destroy(entradasalida_config);
        log_destroy(entradasalida_log);
        log_destroy(log_aux);
        free(nombre_interfaz);
        return 1;
    }
    pthread_join(hilo, NULL);
    return 0;
}

void extraer_segun_tipo_io(char* tipo_interfaz){
    if(strcmp(tipo_interfaz, "GENERICA") == 0){
        tiempo_unidad_trabajo = config_get_int_value(entradasalida_config, "TIEMPO_UNIDAD_TRABAJO");
    }
    else if(strcmp(tipo_interfaz, "STDIN") == 0){
        conectar_con_memoria(entradasalida_config);
    }
    else if(strcmp(tipo_interfaz, "STDOUT") == 0 ){
        conectar_con_memoria(entradasalida_config);
    }
    else if(strcmp(tipo_interfaz, "DIALFS") == 0 ){
        tiempo_unidad_trabajo = config_get_int_value(entradasalida_config, "TIEMPO_UNIDAD_TRABAJO");
        path_base_dialfs = config_get_string_value(entradasalida_config, "PATH_BASE_DIALFS");
        block_size = config_get_int_value(entradasalida_config, "BLOCK_SIZE");
        block_count = config_get_int_value(entradasalida_config, "BLOCK_COUNT");
        retraso_compactacion = config_get_int_value(entradasalida_config, "RETRASO_COMPACTACION");
        conectar_con_memoria(entradasalida_config);
        inicio_filesystem();
    }
}

void conectar_con_memoria(){
    char* ip_memoria = config_get_string_value(entradasalida_config, "IP_MEMORIA");  
    char* puerto_memoria = config_get_string_value(entradasalida_config, "PUERTO_MEMORIA");
     if (ip_memoria == NULL || puerto_memoria == NULL) {
        log_error(log_aux, "Error al obtener IP_MEMORIA o PUERTO_MEMORIA del config");
        return;
    }

    conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    if (conexion_memoria < 0) {
        log_error(log_aux, "Error al conectar con MEMORIA");
        return;
    }
    log_debug(log_aux, "I/O conectado a MEMORIA");
    send_handshake(conexion_memoria, log_aux, "I/O / MEMORIA");
}
