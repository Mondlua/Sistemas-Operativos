#include "utils_consola.h"

t_pcb *crear_nuevo_pcb(uint32_t *pid_contador)
{

    t_pcb *nuevo_pcb = malloc(sizeof(t_pcb));

    if (nuevo_pcb == NULL)
    {
        log_warning(kernel_log, "Se creo un PCB NULL\n");
        return NULL;
    }

    nuevo_pcb->pid = *pid_contador;
    nuevo_pcb->p_counter = 0; 
    nuevo_pcb->quantum = config_get_int_value(kernel_config, "QUANTUM");
    inicializar_registro(nuevo_pcb);
    nuevo_pcb->registros->AX = 5;
    nuevo_pcb->registros->BX = 2;
    nuevo_pcb->tabla_paginas = NULL;
    nuevo_pcb->algoritmo_planif = config_get_string_value(kernel_config, "ALGORITMO_PLANIFICACION");
    nuevo_pcb->estado = NEW;

    log_info(kernel_log, "Se crea el proceso con PID = %u en NEW", nuevo_pcb->pid);
    log_info(kernel_log, "Se crea el proceso con ax = %u en NEW", nuevo_pcb->registros->AX);
    (*pid_contador)++;
    
    return nuevo_pcb;
}

void inicializar_colas_estados()
{
    colaNew = queue_create();
    colaReady = queue_create();
    colaExec = queue_create();
    colaBlocked = queue_create();
    colaExit = queue_create();
}

void liberar_pcb(t_pcb *pcb)
{
    if (pcb != NULL)
    {
        free(pcb->tabla_paginas);
        free(pcb);
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
    default:
        return 0;
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

    t_paquete* paquete = crear_paquete();
    paquete->codigo_operacion= PCB;
    agregar_a_paquete(paquete, &(pcb->pid), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->p_counter), sizeof(int));
    agregar_a_paquete(paquete, &(pcb->quantum), sizeof(int));

    agregar_a_paquete(paquete, (pcb->registros), sizeof(cpu_registros));
    agregar_a_paquete(paquete, &(pcb->estado), sizeof(t_proceso_estado));
    agregar_a_paquete(paquete, (pcb->algoritmo_planif), sizeof(pcb->algoritmo_planif));

    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

void inicializar_registro(t_pcb* pcb)
{

    pcb->registros= malloc(sizeof(cpu_registros));
    pcb->registros->PC = malloc(sizeof(uint32_t));
    pcb->registros->AX = malloc(sizeof(uint8_t));
    pcb->registros->BX = malloc(sizeof(uint8_t));
    pcb->registros->CX = malloc(sizeof(uint8_t));
    pcb->registros->DX = malloc(sizeof(uint8_t));
    pcb->registros->EAX = malloc(sizeof(uint32_t));
    pcb->registros->EBX = malloc(sizeof(uint32_t));
    pcb->registros->ECX = malloc(sizeof(uint32_t));
    pcb->registros->EDX = malloc(sizeof(uint32_t));
    pcb->registros->SI = malloc(sizeof(uint32_t));
    pcb->registros->DI = malloc(sizeof(uint32_t));
}

/*void cambiar_cola(t_pcb* pcb,t_proceso_estado nuevo_estado){
    t_queue* cola_actual= cola_de_estado(pcb->estado);
    queue_pop();
    pcb->estado=nuevo_estado;
}*/