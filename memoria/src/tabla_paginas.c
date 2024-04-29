#include "tabla_paginas.h"

tabla_paginas *inicializar_tabla(int tam_pagina, int tam_memoria) {
    tabla_paginas* tabla=malloc(sizeof(tabla_paginas));
    tabla->cant_paginas = tam_memoria / tam_pagina;
    tabla->paginas = malloc(tabla->cant_paginas * sizeof(pagina));

    pagina *pag_actual = tabla->paginas; 
    int numdepag = 0;
    
    while (numdepag < tabla->cant_paginas) {
        pag_actual->nro_pagina = numdepag;
        numdepag++;
        pag_actual->data = malloc(tam_pagina);
        pag_actual = (pagina *)((char*)pag_actual + sizeof(pagina)); 
    }
    return tabla;
}