#ifndef PLANNER_DISCORDIADOR_PLANNER_H_
#define PLANNER_DISCORDIADOR_PLANNER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <commons/string.h>
#include <string.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/error.h>
#include <math.h>
#include <semaphore.h>
#include "/home/utnso/Documentos/tp-2021-1c-fajumanito/discoridador/src/tripulantes/tareas.h"
#include "/home/utnso/Documentos/tp-2021-1c-fajumanito/Shared-Commons/serializador.h"
#include "/home/utnso/Documentos/tp-2021-1c-fajumanito/Shared-Commons/logger.h"
#include "/home/utnso/Documentos/tp-2021-1c-fajumanito/Shared-Commons/conexiones.h"
#include "/home/utnso/Documentos/tp-2021-1c-fajumanito/Shared-Commons/lista.h"

typedef enum
{
    FIFO,
    RR
}enum_algoritmo;

typedef struct config{
    char* IP_MI_RAM_HQ;
    uint32_t PUERTO_MI_RAM_HQ;
    char* IP_I_MONGO_STORE;
    uint32_t PUERTO_I_MONGO_STORE;
    int GRADO_MULTITAREA;
    char* ALGORITMO;
    int QUANTUM;
    int DURACION_SABOTAJE;
    int RETARDO_CICLO_CPU;
}t_tpConfig;

typedef struct {
    uint32_t id;
    char estado; //"n" "r" "b" "e" "s" //s es para bloques por sabotaje
    uint32_t posX;
    uint32_t posY;
    uint32_t dir_sig_instruccion;
    uint32_t dir_patota;
} t_tripulante_mensaje;

typedef struct {
    char* nombre;
    int posX;
    int posY;
    int ciclos_de_cpu; //duracion
} t_tarea; //LOS SABOTAJES TIENEN LA MISMA ESTRUCTURA

/*
typedef struct {
    char* clase;
    int posX;
    int posY;
    int ciclos_de_cpu; //duracion
} t_sabotaje; //ES IGUAL A TAREA PERO QUEDA PARA CLARIDAD EN CODIGO
*/

typedef struct {
    int id;
    int id_patota;
    char estado; //"n" "r" "b" "e"
    int posX;
    int posY;
    //int dir_sig_instruccion;
    int dir_patota;
    t_tarea tareaActual;
    sem_t semaforoDeTripulante;
    int ciclos_cpu_ejecutados;
} t_tripulante;

typedef struct {
    int cliente;
    t_log* log;
} p_elementoDeHilo;

//////////SEMAFOROS//////////
sem_t sem_tripulantes_disponibles;
sem_t semaforoPlanifiquenme;
sem_t sem_message_on_queue;
sem_t sem_tripulantes_en_ready_queue;
sem_t sem_tareas_a_realizar;
sem_t sem_planificador;
sem_t semaforoSocketMIRAMHQ;
sem_t semaforoConexionAMIRAMHQ;
sem_t semaforoCambioDeEstado;
sem_t semaforoMIRAMHQ;
sem_t semaforoSocket;
sem_t semaforoFILESYSTEM;
//sem_t semaforoSocketFILESYSTEM;
sem_t mutex_FIFO;
sem_t mutex_RR;
sem_t mutex_blocked;
sem_t mutex_ready;
sem_t mutex_exit;
sem_t mutex_exec;
sem_t mutex_blocked_sabotaje;
sem_t semaforoTareas;
sem_t semaforoTermine;

////////HILOS/////////
pthread_t hiloPlanificacion;
pthread_t planificador;
pthread_t algoritmo_cercania_tripulantes;
pthread_mutex_t cola_tareas_a_realizar;
pthread_mutex_t cola_exec;
pthread_t *servidor;

//////LOGS//////
t_log *discordiador_log;
t_log *fileSystem_log;

//SETUP

//t_list *tripulantes_libres;
t_list *lista_imprimir_tripulantes;
t_list *TAREAS_PENDIENTES;
t_config* creacionConfig;
t_tpConfig* tpConfig;

//listas
t_list* listaReady;
t_list *listaNew;
t_list *listaBlocked;
t_list *listaExec;
t_list *listaExit;

t_list* tarea_a_realizar;
t_list* total_targets_tareas;
t_list* message_catch_sended;
t_list* tareas_pendientes;
t_list* tiene_tareas;
t_list* lista_auxiliar;
t_list* real_targets_tareas;
t_list* get_id_corr;

//falta el de lista bloqueados
int grado_multiprocesamiento;
int tripulantes_trabajando;
int tiempo_retardo;
int quantum;
char *algoritmo;
bool liberar;
void ALGORITMO_FIFO();
bool SEGUIR_ATENDIENDO;
bool HAY_SABOTAJE;
int length;
double buffer;
unsigned long int CLOCK;

//FUNCIONES
void inicializar(int argc, char *argv[]);
int iniciarConexionAMIRAMHQ();
uint32_t recibirResponse(int conexion, HeaderAmongOS headerACK);
void iniciarServidorMIRAMHQ(pthread_t* servidor); //SACAR
void* atenderMIRAMHQ();
void* recibirYAtenderUnClienteMIRAMHQ(p_elementoDeHilo* elemento);
void* recibirYAtenderUnCliente(p_elementoDeHilo* elemento);
void destruirTarea(t_tarea *tarea);
void borrarTareaDePendientes(char *tarea);
void sacarTarea(int idTripilante);
void finalizar();
void inicializarConfig(int argc, char *argv[]);
void inicializar_semaforos();
void crear_hilo_planificacion();
void enviar_ready(t_tripulante *tripulante);
void *pasar_a_block(void *proceso);
void planificar_segun(char* tipoPlanif);
void borrar_de_new_queue(t_tripulante* tripulante);
void enviar_ready(t_tripulante *tripulante);
void desplazar_tripulante(t_tripulante *tripulante);
bool llegoATarea(t_tripulante *tripulante);
t_tripulante* discordiador_exec_tripulante(t_tripulante* tripulante);
void sumarCiclos(t_tripulante *tripulante, int ciclos);
void avisoTareaTerminada(t_tripulante *tripulante);
void ponerAlFinalDeLista(t_tripulante *tripulante, t_list *lista);
void algoritmo_RR();
bool noHayMasTareasTripulante(t_tripulante *tripulante);
bool _ordenarPorId(void* element1,void* element2);
void iniciar_patota(char *iniciacion);
void iniciar_tripulante(int id_tripulante, char estado_tripulante, int posX, int posY, int id_patota);
void expulsar_tripulante(int id_tripulante);
void listar_tripulantes();
bool condicion_id_mayor_listar_tripulantes(t_tripulante elemento1,t_tripulante elemento2);
void imprimir_lista(t_list *lista);
void iniciarVariablesDePlanificacion();
void cargoLosDeReady(t_list *colaTodos,t_list *colaAVaciar);
void cargoLosDeExec(t_list *colaTodos,t_list *colaAVaciar);
void cargoLosDeBlocked(t_list *colaTodos,t_list *colaAVaciar);
void* recibirYAtenderFileSystem(p_elementoDeHilo* elemento);
int diferenciaEntrePuntos( int tripulanteposX, int tripulanteposY, int sabotajeposX, int sabotajeposY);
void atender(HeaderAmongOS header, int cliente, t_log* logger);
void atenderSabotaje(t_tarea unSabotaje);
void pausar_semaforos();
void reanudar_semaforos();
t_tripulante* tripulanteMasCercano(t_tarea sabotaje, t_list *cola);
void pausar_tripulantes(t_list *colaBlockedSabotaje);
void reanudar_tripulantes(t_list *colaBlockedSabotaje);


#endif /* PLANNER_DISCORDIADOR_PLANNER_H_ */
