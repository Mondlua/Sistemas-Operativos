#include <main.h>

t_log *kernel_log;
t_config *kernel_config;

t_queue *colaNew;
t_queue *colaReady;
t_queue *colaExec;
t_queue *colaBlocked;
t_queue *colaExit;

int main(void)
{

    // t_log* kernel_log;
    // t_config* kernel_config;
    int kernel_server;
    char *puerto_escucha;

    char *ip_memoria;
    char *puerto_memoria;
    int conexion_memoria;

    char *ip_cpu;
    char *puerto_cpu_dispatch;
    char *puerto_cpu_interrupt;

    int conexion_cpu_dispatch;
    int conexion_cpu_interrupt;

    kernel_log = iniciar_logger("kernel.log", "kernel");
    kernel_config = iniciar_config("kernel.config");

    /* KERNEL - Cliente */

    // Extraer configs
    
    ip_memoria = config_get_string_value(kernel_config,"IP_MEMORIA");
    puerto_memoria = config_get_string_value(kernel_config, "PUERTO_MEMORIA");

    ip_cpu = config_get_string_value(kernel_config, "IP_CPU");
    puerto_cpu_dispatch = config_get_string_value(kernel_config, "PUERTO_CPU_DISPATCH");
    puerto_cpu_interrupt = config_get_string_value(kernel_config, "PUERTO_CPU_INTERRUPT");
    
    // Establecer conexiones
    
        conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
        log_info(kernel_log, "KERNEL se conectó a MEMORIA");
        send_handshake(conexion_memoria, kernel_log, "KERNEL / MEMORIA");

        conexion_cpu_dispatch = crear_conexion(ip_cpu, puerto_cpu_dispatch);
        log_info(kernel_log, "KERNEL se conectó a CPU DISPATCH");
        send_handshake(conexion_cpu_dispatch, kernel_log, "KERNEL / CPU DISPATCH");

        conexion_cpu_interrupt = crear_conexion(ip_cpu, puerto_cpu_interrupt);
        log_info(kernel_log, "KERNEL se conectó a CPU INTERRUPT");
        send_handshake(conexion_cpu_interrupt, kernel_log, "KERNEL / CPU INTERRUPT");

        
    /* KERNEL - Servidor */
    
    // Extraer configs

    puerto_escucha = config_get_string_value(kernel_config, "PUERTO_ESCUCHA");

    // Inicio server
    
        kernel_server = iniciar_servidor(puerto_escucha, kernel_log);
        log_info(kernel_log, "KERNEL listo para recibir clientes");
        server_escuchar(kernel_log, "kernel", kernel_server);

        
    /* KERNEL - PCB */

    // Inicializo Colas de Estado //ver  si es necesario para execute y exit

    inicializar_colas_estados();
    t_pcb* nuevo_pcb = iniciar_proceso();
    //iniciar_proceso();
    enviar_pcb_cpu(nuevo_pcb,conexion_cpu_dispatch);
/*
    uint32_t x=1;
    //uint32_t y = 0;

    proceso_estado();
    finalizar_proceso(x);
    proceso_estado();
*/

    return 0;
}

// INICIAR PROCESO //

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

t_pcb *crear_nuevo_pcb(uint32_t *pid_contador)
{

    t_pcb *nuevo_pcb = malloc(sizeof(t_pcb));

    if (nuevo_pcb == NULL)
    {
        log_warning(kernel_log, "Se creo un PCB NULL\n");
        return NULL;
    }

    nuevo_pcb->pid = *pid_contador;
    nuevo_pcb->p_counter = 0; // ver de donde salen todas las instrucciones
    nuevo_pcb->quantum = config_get_int_value(kernel_config, "QUANTUM");
    nuevo_pcb->tabla_paginas = NULL;
    nuevo_pcb->algoritmo_planif = config_get_string_value(kernel_config, "ALGORITMO_PLANIFICACION");
    nuevo_pcb->estado = NEW;

    log_info(kernel_log, "Se crea el proceso con PID = %u en NEW", nuevo_pcb->pid);

    (*pid_contador)++;

    return nuevo_pcb;
}

void liberar_pcb(t_pcb *pcb)
{
    if (pcb != NULL)
    {
        free(pcb->tabla_paginas);
        free(pcb);
    }
}

////CAMBIO MULTIPROGRAMACION /////

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

void inicializar_colas_estados()
{
    colaNew = queue_create();
    colaReady = queue_create();
    colaExec = queue_create();
    colaBlocked = queue_create();
    colaExit = queue_create();
}

void proceso_estado()
{
    for (t_proceso_estado estado = NEW; estado <= EXIT; estado++)
    {
        mostrar_pids_en_estado(estado);
    }
}

t_queue *cola_de_estado(t_proceso_estado estado)
{

    switch (estado)
    {
    case 0:
        return colaNew;
    case 1:
        return colaReady;
    case 2:
        return colaExec;
    case 3:
        return colaBlocked;
    case 4:
        return colaExit;
    }
}

void mostrar_pids_en_estado(t_proceso_estado estado)
{

    t_queue *cola = cola_de_estado(estado);
    int tamanio_cola = queue_size(cola);
    int contador = 0;

    if (tamanio_cola != 0)
    {
        log_info(kernel_log, "Los PID en la COLA %s son:", estado_a_string(estado));

        while (contador < tamanio_cola)
        {

            t_pcb *pcb_apunta = queue_pop(cola);
            uint32_t pid = pcb_apunta->pid;
            queue_push(cola, pcb_apunta);
            log_info(kernel_log, "PID: %u", pid);

            contador++;
        }
    }
    else
    {
        log_warning(kernel_log, "La COLA %s esta VACIA", estado_a_string(estado));
    }
}

/// funcion para especificar en LOG cual elemento del enum es (estado)
char *estado_a_string(t_proceso_estado estado)
{
    switch (estado)
    {
    case 0:
        return "NEW";
    case 1:
        return "READY";
    case 2:
        return "EXEC";
    case 3:
        return "BLOCKED";
    case 4:
        return "EXIT";
    }
}

void finalizar_proceso(uint32_t num_pid)
{
    borrar_pcb(num_pid);
}

// Devuelve 1. TRUE o 0. FALSE si encuentra el elemento en la cola COMMONS QUEUE //Ver
int find_queue(uint32_t elem, t_queue *cola)
{
    if (queue_is_empty(cola))
    {
        return 0;
    }

    t_link_element *actual = cola->elements->head;

    while (actual != NULL)
    {
        uint32_t *dato = (uint32_t*)actual->data;
        if (*dato == elem)
        {
            return 1;
        }
        actual = actual->next;
    }
    return 0;
}

t_queue* cola_pcb(uint32_t num_pid){ //buscar cola en cada estado

    t_queue* buscado = NULL;
    for (t_proceso_estado estado = NEW; estado <= EXIT; estado++)
    {
        t_queue* cola = cola_de_estado(estado);
        if (find_queue(num_pid, cola) == 1)
        {
            buscado = cola;
        }
    }
    if(buscado == NULL){
        log_error(kernel_log, "No se pudo encontrar el PCB de PID: %u", num_pid);
    }
    return buscado;
}

void borrar_pcb(uint32_t num_pid)
{
    t_queue* cola = cola_pcb(num_pid);
    int tamanio_cola = queue_size(cola);
    int contador = 0;

    while (contador < tamanio_cola)
        {

            t_pcb *pcb_apunta = queue_pop(cola);

            if (pcb_apunta->pid == num_pid)
            {
                // No lo vuelve a encolar 
                // Ver liberar memoria, recursos, y archivos
            }
            else
            {
                queue_push(cola, pcb_apunta);
            }
            contador++;
        }
}

t_pcb* buscar_pcb(uint32_t num_pid){

    t_queue* cola = cola_pcb(num_pid);
    int tamanio_cola = queue_size(cola);
    int contador = 0;

    t_pcb* buscado;

    while (contador < tamanio_cola)
        {
            t_pcb *pcb_apunta = queue_pop(cola);

            if (pcb_apunta->pid == num_pid)
            {
                buscado = pcb_apunta;
            }
            queue_push(cola, pcb_apunta);
            contador++;
        }
    return buscado;
}

void enviar_pcb_cpu(t_pcb* pcb, int socket_cliente){

   /* t_pcb* pcb = buscar_pcb(num_pid);
    t_paquete* paquete_pcb = crear_paquete();
    agregar_a_paquete(paquete_pcb, pcb, sizeof(pcb));
*/
    // CONTINUAR
    t_paquete* paquete = crear_paquete();
    agregar_a_paquete(paquete, &(pcb->pid), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->p_counter), sizeof(int));
    agregar_a_paquete(paquete, &(pcb->quantum), sizeof(int));
    agregar_a_paquete(paquete, &(pcb->registros), sizeof(int));
    agregar_a_paquete(paquete, &(pcb->estado), sizeof(t_proceso_estado));
    agregar_a_paquete(paquete, pcb->tabla_paginas, sizeof(int) * pcb->registros);
    agregar_a_paquete(paquete, pcb->algoritmo_planif, sizeof(pcb->algoritmo_planif));

    enviar_paquete(paquete, socket_cliente);

    eliminar_paquete(paquete);

}