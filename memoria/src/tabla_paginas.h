#ifndef TABLA_H
#define TABLA_H
#include "main.h"

typedef struct pagina{
    int nro_pagina;
    void* data;
}pagina;

typedef struct tabla_paginas{
    int cant_paginas;
    pagina *paginas;
}tabla_paginas;

tabla_paginas* inicializar_tabla(int tam_pagina, int tam_memoria);

#endif