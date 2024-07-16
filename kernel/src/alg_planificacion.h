#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

//#include "main.h"
#include <utils/mensajesPropios.h>
#include <commons/temporal.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <commons/string.h>


extern sem_t grado_planificiacion;
extern sem_t cola_ready;

typedef struct {
    char *identificador;
    t_queue *block_queue;
    int cantidad_instancias;
    int socket_interfaz;

} t_queue_block;

typedef struct {
    t_queue *new;
    t_queue *ready;
    t_queue *exec;
    t_dictionary *lista_block;
    int cantidad_procesos_block;
    t_queue *exit;
    t_queue *prioridad;
} t_planificador_colas;

typedef struct{
    int puerto_escucha;
    char *ip_memoria;
    int puerto_memoria;
    char *ip_cpu;
    char *puerto_cpu_dispatch;
    char *puerto_cpu_interrupt;
} t_config_leida_kernel;

typedef struct{
    t_config_leida_kernel config_leida;
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
typedef enum {
    FIFO,
    RR,
    VRR
}t_tipo_planificacion;
typedef struct
{
    t_log *logger;
    t_planificador_colas colas;
    t_planificador_config config;
    sem_t planificar;
    int detener_planificacion;
    t_timer_planificador* timer_quantum;
    int socket_cpu_dispatch;
    int socket_cpu_interrupt;
    int socket_memoria;
    t_tipo_planificacion algo_planning;
} t_planificacion;

void* hilo_planificador(void *args);

void fifo(t_planificacion *kernel_argumentos);
void round_robin(t_planificacion *kernel_argumentos);
void virtual_round_robin(t_planificacion *kernel_argumentos);

void enviar_interrupcion(union sigval sv);
void iniciar_timer(t_timer_planificador* timer, int milisegundos);
int frenar_timer(t_timer_planificador* timer);
timer_t inicializar_timer(t_planificacion *kernel_argumentos);

void planificador_planificar(t_planificacion *kernel_argumentos);
bool planificador_recepcion_pcb(t_pcb *pcb_desalojado, t_planificacion *kernel_argumentos);

void planificador_corto_plazo(t_tipo_planificacion algoritmo, t_planificacion *kernel_argumentos);
void planificador_largo_plazo(t_planificacion *kernel_argumentos);


t_planificacion *inicializar_t_planificacion(t_config *kernel_config, t_log *kernel_log);
t_tipo_planificacion obtener_algoritmo_planificador(char* algoritmo);
void inicializar_lista_recursos(t_planificacion *planificador, t_config *kernel_config);
void inicializar_config_kernel(t_planificacion *planificador, t_config *kernel_config);


t_pcb *planificador_ready_a_exec(t_planificacion *kernel_argumentos);
t_pcb *planificador_prioridad_a_exec(t_planificacion *kernel_argumentos);

bool administrador_recursos_wait(t_pcb *pcb_solicitante, char* nombre_recurso, t_planificacion *kernel_argumentos);
bool administrador_recursos_signal(t_pcb *pcb_desalojado, char* recurso_solicitado, t_planificacion *kernel_argumentos);
void procesar_desbloqueo_factible(char* recurso_solicitado, t_planificacion *kernel_argumentos);

#endif