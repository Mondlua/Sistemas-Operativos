#ifndef GLOBAL_H
#define GLOBAL_H

#include <commons/collections/list.h>
#include <semaphore.h>

// Declaración externa de la variable global interfaces
extern t_list* interfaces;
extern sem_t sem_contador;

#endif
