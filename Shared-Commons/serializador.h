#ifndef SERIALIZACION_SERIALIZACION_H_
#define SERIALIZACION_SERIALIZACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

/////////////////////////////////////////
//       Como usar esta libreria       //
/////////////////////////////////////////
//                                     //
// - Para enviar se utiliza Serialize  //
// _PackAndSend_ y el tipo de paquete  //
// correspondiente                     //
//                                     //
// - Para recibir algo primero se      //
// utiliza receiveHeader y despues     //
// con los datos obtenidos, se llama a //
// receive and unpack obteniendo asi   //
// el pack void*                       //
//                                     //
// - Para extraer datos especificos    //
// de estos paquetes se utilizan las   //
// funciones unpack                    //
//                                     //
// - NUNCA llamar a una funcion unpack //
// no correspondiente al tipo de pack  //
// que recibimos                       //
//                                     //
/////////////////////////////////////////

///////////////////////////////////////////////////////
//           Tipos de mensajes que enviamos         //
/////////////////////////////////////////////////////
typedef enum d_messages {
    d_TRIPULANTE,
    d_TAREA,
    d_NEW_PATOTA,
    d_OBTENER_BITACORA,
    d_ELIMINAR_TRIPULANTE,
    d_IMPRIMIR_TRIPULANTES,
    d_TRIPULANTE_POS_NUEVA_X,
    d_TRIPULANTE_POS_NUEVA_Y,
    d_TRIPULANTE_ESTADO_NUEVO,
    d_DAME_TRIPULANTE,
    d_INICIO_TAREA,
    d_FIN_TAREA,
    d_SUBSCRIBE_QUEUE,
    d_ACK,
    d_SABOTAJE
} d_message;
//d_TRIPULANTE 128B, EL RESTO DEL

typedef enum d_process {
	d_DISCORDIADOR,
	d_MIRAMHQ,
	d_IMONGOSTORE
} d_modulos;

////////////////////////
// Comunicacion Base //
///////////////////////

typedef struct {
	uint32_t tamanioMensaje;
	d_message tipoMensaje;
}__attribute__((packed)) HeaderAmongOS; //Esta estructura es de tamaño 8 A M O G U S

typedef struct {
	uint32_t posX;
	uint32_t posY;
	//uint32_t cantidad;
}__attribute__((packed)) d_PosCant;

////////////////
// FUNCIONES //
///////////////

///////////////////////////
// FUNCIONES PARA ENVIAR //
///////////////////////////

/**
* ESTA FUNCION ENVIA UN PAQUETE DEL TAMAÑO ESPECIFICADO A TRAVES DEL SOCKET ESPECIFICADO
* ES LA FUNCION DEFAULT QUE USAMOS PARA ENVIAR UN PAQUETE Y LAS OTRAS FUNCIONES DE ENVIAR
* PAQUETES LLAMARAN A ESTA
*/

bool Serialize_PackAndSend(int socketCliente, const void*pack, uint32_t tamPack, d_message tipoMensaje);

/**
* ESTA FUNCION NOS SIRVE PARA CONTESTAR ACK
*/

bool Serialize_PackAndSend_ACK(int socketCliente, uint32_t miId);

/**
 * ESTA FUNCION NOS SIRVE PARA PEDIR SUSCRIBIRNOS A UNA COLA
 */

bool Serialize_PackAndSend_SubscribeQueue(int socketCliente, d_message queue);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO TAREA A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_TAREA(int socketCliente, const void *tarea, uint32_t posX, uint32_t posY, uint32_t cantidad);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO PATOTA A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_NEW_PATOTA(int socketCliente, uint32_t patota_id, char *string);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO TRIPULANTE A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_TRIPULANTE(int socketCliente, uint32_t tripulante_id, char estado, uint32_t posX, uint32_t posY, uint32_t prox_tarea, uint32_t puntero_PCB);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO TRIPULANTE_ID A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_TRIPULANTE_ID(int socketCliente, uint32_t tripulante_id, d_message tipoMensaje);

/**
 * ESTAS FUNCIONES ENVIAN UN PAQUETE DEL TIPO TRIPULANTE_ID A TRAVES DEL SOCKET ESPECIFICADO
 * PERO HACEN COSAS DIFERENTES POR ESO ESTAN SEPARADAS
 */

bool Serialize_PackAndSend_OBTENER_BITACORA(int socketCliente, uint32_t tripulante_id);

bool Serialize_PackAndSend_ELIMINAR_TRIPULANTE(int socketCliente, uint32_t tripulante_id);

bool Serialize_PackAndSend_TRIPULANTE_NEW_POSX(int socketCliente, uint32_t tripulante_id, uint32_t nuevoPosX,uint32_t posVieja);

bool Serialize_PackAndSend_TRIPULANTE_NEW_POSY(int socketCliente, uint32_t tripulante_id, uint32_t nuevoPosY,uint32_t posVieja);

bool Serialize_PackAndSend_TRIPULANTE_NEW_ESTADO(int socketCliente, uint32_t tripulante_id, char nuevoEstado);

bool Serialize_PackAndSend_TRIPULANTE_REQUEST(int socketCliente, char estado);

bool Serialize_PackAndSend_IMPRIMIR_TRIPULANTE(int socketCliente, d_message tipoMensaje);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO SABOTAJE A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_SABOTAJE(int socketCliente, uint32_t tipoSabotaje, uint32_t posX, uint32_t posY, uint32_t duracion);


////////////////////////////
// FUNCIONES PARA RECIBIR //
////////////////////////////

/**
* ESTA FUNCION RETORNA UNA ESTRUCTURA DEL TIPO
* HEADERDELIBIRD DE LA CUAL PODEMOS OBTENER EL TIPO
* DE MENSAJE Y EL TAMANIO DEL MISMO, ESTE ULTIMO
* PARAMETRO DEBEREMOS PASARSELO A LA FUNCION
* Serialize_ReceiveAndUnpack PARA QUE NOS DE EL RESTO
* DEL PAQUETE Y DESPUES DE ESO EL RESTO DEL PAQUETE PASARSELO
* A LAS FUNCIONES DE DESEMPAQUETADO CORRESPONDIENTE
*/

HeaderAmongOS Serialize_RecieveHeader(int socketCliente);

/**
* ESTA FUNCION RECIBE UN PAQUETE A TRAVES DEL
* SOCKET, TENER EN CUENTA QUE SEGUN EL TIPO DE
* OPERACION, EL PAQUETE RECIBIDO SERA DISTINTO
* PARA DESEMPAQUETAR APROPIADAMENTE HARA FALTA
* LLAMAR A OTRAS FUNCIONES DE DESEMPAQUETADO
*/

void* Serialize_ReceiveAndUnpack(int socketCliente, uint32_t tamanioChar);

//////////////////////////////////
// FUNCIONES PARA DESEMPAQUETAR //
/////////////////////////////////

/**
* ESTA FUNCION DESEMPAQUETA EL ENTERO DE UN
* PAQUETE ACK, ESTE ENTERO REPRESENTARA EL
* ESTADO DE LA OPERACION
*/

uint32_t Serialize_Unpack_ACK(void *pack);

/**
 * ESTA FUNCION DESEMPAQUETA EL ID DEL MENSAJE DE UN
 * PAQUETE VOID* QUE LO CONTENGA
 * NO USAR ESTA FUNCION EN PAQUETES QUE NO
 * CONTENGAN DICHO DATO
 */

uint32_t Serialize_Unpack_idMensaje(void *pack);

/**
 * ESTA FUNCION DESEMPAQUETA EL NOMBRE DE LA TAREA
 * PAQUETE VOID* QUE LO CONTENGA
 * NO USAR ESTA FUNCION EN PAQUETES QUE NO
 * CONTENGAN DICHO DATO
 * DE LA MISMA MANERA PARA CADA DATO DE LA TAREA
 */

char* Serialize_Unpack_TareaName(void *buffer);

uint32_t Serialize_Unpack_posX(void *pack);

uint32_t Serialize_Unpack_posY(void *pack);

uint32_t Serialize_Unpack_cantidad(void *pack);

uint32_t Serialize_Unpack_PATOTA_ID(void *pack);

char* Serialize_Unpack_TODAS_LAS_TAREAS(void *buffer);

uint32_t Serialize_Unpack_TRIPULANTE_ID(void *pack);

char* Serialize_Unpack_TRIPULANTE_ESTADO(void *buffer);

uint32_t Serialize_Unpack_posX_TRIPULANTE(void *pack);

uint32_t Serialize_Unpack_posY_TRIPULANTE(void *pack);

uint32_t Serialize_Unpack_PROX_TAREA(void *pack);

uint32_t Serialize_Unpack_PUNTERO_PCB(void *pack);

uint32_t Serialize_Unpack_resultado(void *pack);

uint32_t Serialize_Unpack_NUEVA_POS_TRIPULANTE(void *pack);

char* Serialize_Unpack_TRIPULANTE_ESTADO_NUEVO(void *buffer);

char* Serialize_Unpack_DAME_TRIPULANTES(void *buffer);

//PARA LOS SABOTAJES
char* Serialize_Unpack_TIPO_SABOTAJE(void *buffer);

char* Serialize_Unpack_POSX_SABOTAJE(void *buffer);

char* Serialize_Unpack_POSY_SABOTAJE(void *buffer);

char* Serialize_Unpack_DURACION_SABOTAJE(void *buffer);

//////////////////////////////////////
// FUNCIONES PARA DESEMPAQUETAR PRO //
/////////////////////////////////////

/*
 * La intencion de las funciones de desempaquetado pro
 * es facilitar el desempaquetado de los paquetes recibidos
 */

/*
 * ESTA FUNCION DADO LOS PARAMETROS PASADOS POR REFERENCIA Y
 * EL PAQUETE VOID* LLENA ESOS PARAMETROS CON LOS VALORES ADENTRO
 * DEL PAQUETE DE TIPO NEW_POKEMON (SOLO SIRVE PARA ESE TIPO DE PAQUETE)
 */

void Serialize_Unpack_TAREA(void *packTarea, uint32_t *idMensaje, char *nombre, uint32_t posX, uint32_t posY, uint32_t cantidad);

/*
 * ESTA FUNCION DADO LOS PARAMETROS PASADOS POR REFERENCIA Y
 * EL PAQUETE VOID* LLENA ESOS PARAMETROS CON LOS VALORES ADENTRO
 * DEL PAQUETE DE TIPO CATCH_POKEMON (SOLO SIRVE PARA ESE TIPO DE PAQUETE)
 */

void Serialize_Unpack_PATOTA(void *packPatotaNew, uint32_t *idMensaje, uint32_t patota_id, char *todasLasTareas);

/*
 * ESTA FUNCION DADO LOS PARAMETROS PASADOS POR REFERENCIA Y
 * EL PAQUETE VOID* LLENA ESOS PARAMETROS CON LOS VALORES ADENTRO
 * DEL PAQUETE DE TIPO GETPOKEMON (SOLO SIRVE PARA ESE TIPO DE PAQUETE)
 */

void Serialize_Unpack_TRIPULANTE(void *packTripulante, uint32_t *idMensaje, uint32_t tripulante_id, uint32_t posX, uint32_t posY, uint32_t prox_tarea, uint32_t puntero_PCB);

/*
 * ESTA FUNCION DADO LOS PARAMETROS PASADOS POR REFERENCIA Y
 * EL PAQUETE VOID* LLENA ESOS PARAMETROS CON LOS VALORES ADENTRO
 * DEL PAQUETE DE TIPO APPEARED_POKEMON (SOLO SIRVE PARA ESE TIPO DE PAQUETE)
 */

void Serialize_Unpack_CaughtPokemon(void *packTripulanteTrea, uint32_t *idMensaje, uint32_t *resultado);

void Serialize_Unpack_TRIPULANTE_POS_NUEVA_X(void *packTripulante, uint32_t *idMensaje, uint32_t tripulante_id, uint32_t posX);

void Serialize_Unpack_TRIPULANTE_POS_NUEVA_Y(void *packTripulante, uint32_t *idMensaje, uint32_t tripulante_id, uint32_t posY);

void Serialize_Unpack_TRIPULANTE_ESTADO_NUEVO(void *packTripulante, uint32_t *idMensaje, uint32_t tripulante_id, char estado);

void Serialize_Unpack_DAME_TRIPULANTE(void *packTripulante, uint32_t *idMensaje, char estado);

void Serialize_Unpack_SABOTAJE(void *packTripulante, uint32_t *idMensaje, uint32_t tipoSabotaje, uint32_t posX, uint32_t posY, uint32_t duracion);

#endif /* SERIALIZACION_SERIALIZACION_H_ */
