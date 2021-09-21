#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <commons/string.h>
#include <string.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/log.h>
#include <math.h>
#include <semaphore.h>

typedef enum{
	generar_oxigeno,
	consumir_oxigeno,
	generar_comida,
	consumir_comida,
	generar_basura,
	descartar_basura
}enum_tareas;

typedef enum{
	cantidad_bloques,
    bitmap,
    size,
    block_count,
    blocks
}enum_sabotajes;