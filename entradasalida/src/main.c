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

int main(void) {
	t_config* entradasalida_config;   

    char* ip_kernel; 
    char* puerto_kernel;
    
    char* interfaz;

    char ruta[200];
    nombre_interfaz = malloc(sizeof(256));

    entradasalida_log = iniciar_logger("entradasalida.log","entradasalida");
    printf("Ingrese el nombre de la interfaz: ");
    scanf("%s", nombre_interfaz); 
    printf("Ingrese el path del archivo de configuracion: ");
    scanf("%s", ruta);
    
    entradasalida_config = iniciar_config(ruta);
    
	/* I/O - Cliente */

    interfaz = config_get_string_value(entradasalida_config, "TIPO_INTERFAZ");
    ip_kernel= config_get_string_value(entradasalida_config,"IP_KERNEL");
    puerto_kernel = config_get_string_value(entradasalida_config, "PUERTO_KERNEL");
    extraer_segun_tipo_io(entradasalida_config, interfaz);
    conexion_kernel = crear_conexion(ip_kernel, puerto_kernel);
    log_info(entradasalida_log, "I/O conectado a KERNEL");
    send_handshake(conexion_kernel, entradasalida_log, "I/O / KERNEL");

    aviso_segun_cod_op(nombre_interfaz, conexion_kernel, INTERFAZ);

    pthread_t hilo;
    pthread_create(&hilo, NULL, (void *)recibir_instruccion, interfaz);
    pthread_join(hilo, NULL);
    return 0;
}

void extraer_segun_tipo_io(t_config* config, char* tipo_interfaz){
    if(strcmp(tipo_interfaz, "GENERICA") == 0){
        tiempo_unidad_trabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    }
    else if(strcmp(tipo_interfaz, "STDIN") == 0){
        conectar_con_memoria(config);
    }
    else if(strcmp(tipo_interfaz, "STDOUT") == 0 ){
        conectar_con_memoria(config);
    }
    else if(strcmp(tipo_interfaz, "DIALFS") == 0 ){
        tiempo_unidad_trabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
        path_base_dialfs = config_get_string_value(config, "PATH_BASE_DIALFS");
        block_size = config_get_int_value(config, "BLOCK_SIZE");
        block_count = config_get_int_value(config, "BLOCK_COUNT");
        retraso_compactacion = config_get_int_value(config, "RETRASO_COMPACTACION");
        conectar_con_memoria(config);
        inicio_filesystem();
    }
}

void conectar_con_memoria(t_config* config){
    char* ip_memoria = config_get_string_value(config, "IP_MEMORIA");  
    char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    log_info(entradasalida_log, "I/O conectado a MEMORIA");
    send_handshake(conexion_memoria, entradasalida_log, "I/O / MEMORIA");
}
