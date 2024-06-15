#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include "main.h"
#include <utils/mensajesPropios.h>
#include <commons/temporal.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


extern sem_t grado_planificiacion;
extern sem_t cola_ready;
typedef struct {
    t_queue *new;
    t_queue *ready;
    t_queue *exec;
    t_queue *block;
    t_queue *exit;
    t_queue *prioridad;
} t_planificador_colas;

typedef struct{
    int grado_multiprogramacion;
    char *algoritmo_planificador;
    int quantum;
} t_planificador_config;

typedef struct {
    timer_t timer;
    struct sigaction sa;
    struct sigevent sev;
    struct itimerspec its;
} t_timer_planificador;
typedef struct
{
    t_log *logger;
    t_planificador_colas colas;
    t_planificador_config config;
    sem_t planificar;
    int detener_planificacion;
    t_timer_planificador timer_quantum;
    int socket_cpu_dispatch;
    int socket_cpu_interrupt;
} t_planificacion;

typedef enum {
    FIFO,
    RR,
    VRR
}t_tipo_planificacion;

void* hilo_planificador(void *args);

void planificador_planificar();
bool planificador_recepcion_pcb(t_pcb *pcb_desalojado, t_planificacion *kernel_argumentos);

void planificador_corto_plazo(t_tipo_planificacion algoritmo, t_planificacion *kernel_argumentos);
void planificador_largo_plazo(t_planificacion *kernel_argumentos);


t_planificacion *inicializar_t_planificacion(int socket_cpu_dispatch, int socket_cpu_interrupt);
t_tipo_planificacion obtener_algoritmo_planificador(char* algoritmo);

t_pcb *planificador_ready_a_exec(t_planificacion *kernel_argumentos);
t_pcb *planificador_prioridad_a_exec(t_planificacion *kernel_argumentos);


#endif