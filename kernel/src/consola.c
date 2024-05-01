#include "consola.h"

void consola_interactiva(t_log* logger){
   pthread_t consola;
    int resultado_creacion = pthread_create(&consola, NULL, leer_consola, (void *)logger);
    if (resultado_creacion != 0) {
        fprintf(stderr, "Error: No se pudo crear el hilo. Código de error: %d\n", resultado_creacion);
    return 1;
    }
    pthread_detach(consola);
}

void* leer_consola(void *arg) 
{
    t_log *logger = (t_log *)arg;
    char* leido;
    while (1) {  
        leido = readline("> ");
    if (string_is_empty(leido)) {
        break; 
        }
    funciones(leido, logger);
    free(leido);
    }
    pthread_exit(NULL);
}

void funciones(char* leido, t_log* logger) {
    char** funcion;
    funcion = string_split(leido, " ");
    if (string_equals_ignore_case(funcion[0], "EJECUTAR_SCRIPT") && string_array_size(funcion) == 2){
        ejecutar_script(funcion[1], logger);
    } else if (string_equals_ignore_case(funcion[0], "INICIAR_PROCESO") && string_array_size(funcion) == 2) {
        iniciar_proceso(funcion[1], logger);
    } else if (string_equals_ignore_case(funcion[0], "FINALIZAR_PROCESO") && string_array_size(funcion) == 2) {
        finalizar_proceso(atoi(funcion[1]), logger);
    } else if (string_equals_ignore_case(funcion[0], "INICIAR_PLANIFICACION")) {
        iniciar_planificacion(logger);
    } else if (string_equals_ignore_case(funcion[0], "DETENER_PLANIFICACION")) {
        detener_planificacion(logger);
    } else if (string_equals_ignore_case(funcion[0], "MULTIPROGRAMACION") && string_array_size(funcion) == 2) {
        multiprogramacion(funcion[1], logger);
    } else if (string_equals_ignore_case(funcion[0], "PROCESO_ESTADO")) {
        proceso_estado(logger);
    } else if (!string_is_empty(leido)){
        log_error(logger, ">> COMANDO ERRONEO!");
    }
}


void ejecutar_script(char* path, t_log* logger){
    log_info(logger, ">> Se ejecuta el script %s", path);
}
void iniciar_proceso(char* path, t_log* logger){

    // FILE* archivo;
    t_pcb *pcb;

    // archivo = fopen(nombre_archivo, "r");

    /*if (archivo == NULL) {
        log_error(kernel_log, "No se pudo abrir el archivo.\n");
        return;
    }
*/
    static uint32_t pid_contador = 0;

    pcb = crear_nuevo_pcb(&pid_contador);
    queue_push(colaNew, pcb);
    int grado_actual = config_get_int_value(kernel_config, "GRADO_MULTIPROGRAMACION");

    if(nivel_multiprog<grado_actual){
        queue_pop(colaNew,pcb);
        queue_push(colaReady,pcb);
        log_info(kernel_log,"Proceso con pid %u pasado a la cola de Ready",pcb->pid)
    }
    // avisar a memoria

    // fclose(archivo);
    return pcb;
    log_info(logger, ">> Se crea el proceso %s en NEW", path);
}
void finalizar_proceso(uint32_t pid, t_log* logger){
    borrar_pcb(pid);
    log_info(logger, ">> Se finaliza proceso %i", pid);
}
void iniciar_planificacion(t_log* logger){
    log_info(logger, ">> Se inicio la planificacion");
}
void detener_planificacion(t_log* logger){
    log_info(logger, ">> Se detiene la planificacion");
}

void multiprogramacion(char* valor, t_log* logger){
    

    config_set_value(kernel_config, "GRADO_MULTIPROGRAMACION", valor);

    int cambio_ok = config_get_int_value(kernel_config, "GRADO_MULTIPROGRAMACION");

    if (cambio_ok == valor)
    {
        log_info(kernel_log, "Se cambio el grado de multiprogramacion a %d", valor);
        nivel_multiprog = cambio_ok;
    }
    else
    {
        log_warning(kernel_log, "No se pudo cambiar el grado de multiprogramacion");
    }

    log_info(kernel_log, ">> Se cambio el grado de multiprogamacion a %d", valor);
}

void proceso_estado(t_log* logger){

    log_info(logger, ">> Listado de procesos por estado ...");
    for (t_proceso_estado estado = NEW; estado <= EXIT; estado++)
    {
        mostrar_pids_en_estado(estado);
    }
}
