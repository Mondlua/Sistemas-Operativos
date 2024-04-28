#include "f_consola.h"

/* INICIAR_PROCESO[PATH] */

t_pcb* iniciar_proceso(/*const char *nombre_archivo*/)
{

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
    // avisar a memoria

    // fclose(archivo);
    return pcb;
}

/* PROCESO_ESTADO */

void proceso_estado()
{
    for (t_proceso_estado estado = NEW; estado <= EXIT; estado++)
    {
        mostrar_pids_en_estado(estado);
    }
}

/* FINALIZAR_PROCESO[PID] */

void finalizar_proceso(uint32_t num_pid)
{
    borrar_pcb(num_pid);
}

/* MULTIPROGRAMACION[VALOR] */

void multiprogramacion(int nuevo_grado)
{

    config_set_value(kernel_config, "GRADO_MULTIPROGRAMACION", (char *)nuevo_grado);

    int cambio_ok = config_get_int_value(kernel_config, "GRADO_MULTIPROGRAMACION");

    if (cambio_ok == nuevo_grado)
    {
        log_info(kernel_log, "Se cambio el grado de multiprogramacion a %d", nuevo_grado);
    }
    else
    {
        log_warning(kernel_log, "No se pudo cambiar el grado de multiprogramacion");
    }
}
