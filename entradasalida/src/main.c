#include "main.h"

char* nombre_interfaz;
int conexion_kernel;
int conexion_memoria;
int tiempo_unidad_trabajo;

int main(void) {

    t_log* entradasalida_log;
	t_config* entradasalida_config;   

    char* ip_memoria;
    char* puerto_memoria;   

    char* ip_kernel; 
    char* puerto_kernel;
    
    char* interfaz;

    char ruta[100];

    entradasalida_log = iniciar_logger("entradasalida.log","entradasalida");
    printf("Ingrese el nombre de la interfaz: ");
    scanf("%s", nombre_interfaz); 
    printf("Ingrese el path del archivo de configuracion: ");
    scanf("%s", ruta);
    entradasalida_config = iniciar_config(ruta);
    

	/* I/O - Cliente */

    // Extraer configs

	ip_memoria = config_get_string_value(entradasalida_config,"IP_MEMORIA");  
    puerto_memoria = config_get_string_value(entradasalida_config, "PUERTO_MEMORIA");

    ip_kernel= config_get_string_value(entradasalida_config,"IP_KERNEL");
    puerto_kernel = config_get_string_value(entradasalida_config, "PUERTO_KERNEL");
    // Establecer conexiones

    conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    log_info(entradasalida_log, "I/O conectado a MEMORIA");
    send_handshake(conexion_memoria, entradasalida_log, "I/O / MEMORIA");

    conexion_kernel = crear_conexion(ip_kernel, puerto_kernel);
    log_info(entradasalida_log, "I/O conectado a KERNEL");
    send_handshake(conexion_kernel, entradasalida_log, "I/O / KERNEL");

    // Interfaces
    interfaz = config_get_string_value(entradasalida_config, "TIPO_INTERFAZ");
    tiempo_unidad_trabajo = config_get_int_value(entradasalida_config, "TIEMPO_UNIDAD_TRABAJO");
    aviso_segun_cod_op(nombre_interfaz, conexion_kernel, INTERFAZ);
    recibir_instruccion(interfaz);
    terminar_io();

    aviso_segun_cod_op(nombre_interfaz, conexion_kernel, AVISO_DESCONEXION);
    return 0;
}

void terminar_io(){
    char letra;
    printf("Ingresa una letra para detener el programa: ");
    
    while (1) {  // Bucle infinito
        scanf(" %c", &letra);  // Lee un carácter
        
        if (letra >= 'A' && letra <= 'Z') {  // Verifica si es una letra mayúscula
            printf("Deteniendo el programa...\n");
            break;  // Sale del bucle
        }
    }
}