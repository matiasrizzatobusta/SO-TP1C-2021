#include "serializador.h"

///////////////////////////
// FUNCIONES PARA ENVIAR //
///////////////////////////

bool Serialize_PackAndSend(int socketCliente, const void *pack, uint32_t tamPack, d_message tipoMensaje) {

	uint32_t tamMessage = tamPack + sizeof(d_message) + sizeof(uint32_t);
	printf("Size msg: %d \n", tamMessage);
	fflush(stdout);
	void* buffer = malloc(tamMessage);
	int desplazamiento = 0;
	memcpy(buffer, &tipoMensaje ,sizeof(d_message));
	desplazamiento += sizeof(d_message);
	memcpy(buffer+desplazamiento, &tamPack , sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer+desplazamiento, pack, tamPack);
	desplazamiento += tamPack;
		if(desplazamiento != tamMessage){ return (-1); }
	int resultado = send(socketCliente, buffer, tamMessage, 0);
	free(buffer);
	return resultado;
}

bool Serialize_PackAndSend_ACK(int socketCliente, uint32_t miId){
	void* pack = malloc(sizeof(uint32_t));
	memcpy(pack, &miId, sizeof(uint32_t));
	int resultado = Serialize_PackAndSend(socketCliente, pack, sizeof(uint32_t), d_ACK);
	free(pack);
	return resultado;
}

bool Serialize_PackAndSend_SubscribeQueue(int socketCliente, d_message queue){
	void* pack = malloc(sizeof(d_message));
	memcpy(pack, &queue, sizeof(d_message));
	int resultado = Serialize_PackAndSend(socketCliente, pack, sizeof(d_message), d_SUBSCRIBE_QUEUE);
	free(pack);
	return resultado;
}

bool Serialize_PackAndSend_TAREA(int socketCliente, const void *tarea, uint32_t posX, uint32_t posY, uint32_t cantidad) {//faltan variables
	uint32_t tamMessage = strlen(tarea) + 1 + (4*sizeof(uint32_t)); //+1 por el /0
	uint32_t tamNombreTarea = strlen(tarea) + 1;
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer + desplazamiento , &tamNombreTarea, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, tarea, tamNombreTarea);
	desplazamiento += tamNombreTarea;
	memcpy(buffer + desplazamiento, &posX, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &posY, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &cantidad, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, d_TAREA);
	free(buffer);
	return resultado;

}

//ENVIAR PATOTA NUEVA A MEMORIA 
bool Serialize_PackAndSend_NEW_PATOTA(int socketCliente, uint32_t patota_id, char *string) {
	uint32_t tamMessage = 1 + (sizeof(uint32_t)) + strlen(string); //+1 por el /0
	uint32_t todasLasTareas = strlen(string) + 1;
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer + desplazamiento, &patota_id , sizeof(uint32_t)); //1 1b
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, string , todasLasTareas);		//limpiar piso / hola / saludar/ salir 45b
	desplazamiento += todasLasTareas;
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, d_NEW_PATOTA);
	free(buffer);
	return resultado;

}

//ENVIAR UN TRIPULANTE COMPLETO (la uso para triipulantes nuevos y ya existentes. Si es nuevo, prox_tarea y puntero_PCB van vacios)
bool Serialize_PackAndSend_TRIPULANTE(int socketCliente, uint32_t tripulante_id, char estado, uint32_t posX, uint32_t posY, uint32_t prox_tarea, uint32_t puntero_PCB) {
	uint32_t tamMessage = 1 + (5*sizeof(uint32_t)) + (sizeof(char)); //+1 por el /0
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer + desplazamiento, &tripulante_id , sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, estado , sizeof(char));
	desplazamiento += sizeof(char);
	memcpy(buffer + desplazamiento, &posX, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &posY, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, prox_tarea, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, puntero_PCB, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, d_TRIPULANTE);
	free(buffer);
	return resultado;

}

//PARA PEDIR BITACORA / ELIMINAR TRIPULANTE
bool Serialize_PackAndSend_TRIPULANTE_ID(int socketCliente, uint32_t tripulante_id, d_message tipoMensaje){
	uint32_t tamMessage = sizeof(uint32_t);
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, tripulante_id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, tipoMensaje);
	free(buffer);
	return resultado;
}

bool Serialize_PackAndSend_OBTENER_BITACORA(int socketCliente, uint32_t tripulante_id) {
	return Serialize_PackAndSend_TRIPULANTE_ID(socketCliente, tripulante_id, d_OBTENER_BITACORA);
}

bool Serialize_PackAndSend_ELIMINAR_TRIPULANTE(int socketCliente, uint32_t tripulante_id) {
	return Serialize_PackAndSend_TRIPULANTE_ID(socketCliente, tripulante_id, d_ELIMINAR_TRIPULANTE);
}

//PARA modificar la posicion de un tripulante
bool Serialize_PackAndSend_TRIPULANTE_NEW_POS(int socketCliente, uint32_t tripulante_id, uint32_t nuevoPos, uint32_t viejaPos, d_message tipoMensaje){
	uint32_t tamMessage = sizeof(uint32_t);
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, tripulante_id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer, nuevoPos, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer, viejaPos, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, tipoMensaje);
	free(buffer);
	return resultado;
}

bool Serialize_PackAndSend_TRIPULANTE_NEW_POSX(int socketCliente, uint32_t tripulante_id, uint32_t nuevoPosX, uint32_t viejaPosX){
	Serialize_PackAndSend_TRIPULANTE_NEW_POS(socketCliente, tripulante_id, nuevoPosX, viejaPosX, d_TRIPULANTE_POS_NUEVA_X);
}

bool Serialize_PackAndSend_TRIPULANTE_NEW_POSY(int socketCliente, uint32_t tripulante_id, uint32_t nuevoPosY, uint32_t viejaPosY){
	Serialize_PackAndSend_TRIPULANTE_NEW_POS(socketCliente, tripulante_id, nuevoPosY, viejaPosY, d_TRIPULANTE_POS_NUEVA_Y);
}

//PARA modificar EL ESTADO de un tripulante
bool Serialize_PackAndSend_TRIPULANTE_NEW_ESTADO(int socketCliente, uint32_t tripulante_id, char nuevoEstado){
	uint32_t tamMessage = sizeof(uint32_t);
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, &tripulante_id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer, nuevoEstado, sizeof(char));
	desplazamiento += sizeof(char);
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, d_TRIPULANTE_ESTADO_NUEVO);
	free(buffer);
	return resultado;
}

//para pedir un tripulante de una lista de estados
bool Serialize_PackAndSend_TRIPULANTE_REQUEST(int socketCliente, char estado){
	uint32_t tamMessage = sizeof(uint32_t);
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, estado, sizeof(char));
	desplazamiento += sizeof(char);
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, d_DAME_TRIPULANTE);
	free(buffer);
	return resultado;
}

//PARA IMPRIMIR TODOS LOS TRIPULANTES
bool Serialize_PackAndSend_IMPRIMIR_TRIPULANTE(int socketCliente){
	uint32_t tamMessage = sizeof(uint32_t);
	void* buffer = malloc(tamMessage);
	int desplazamiento = 0;
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, d_IMPRIMIR_TRIPULANTES);
	free(buffer);
	return resultado;
}

//PARA modificar la posicion de un tripulante
bool Serialize_PackAndSend_TRIPULANTE_TRABAJO(int socketCliente, uint32_t tripulante_id, d_message tipoMensaje){
	uint32_t tamMessage = sizeof(uint32_t);
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, tripulante_id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, tipoMensaje);
	free(buffer);
	return resultado;
}

bool Serialize_PackAndSend_TRIPULANTE_INICIO_TAREA(int socketCliente, uint32_t tripulante_id){
	Serialize_PackAndSend_TRIPULANTE_TRABAJO(socketCliente,tripulante_id ,d_INICIO_TAREA);
}

bool Serialize_PackAndSend_TRIPULANTE_FIN_TAREA(int socketCliente, uint32_t tripulante_id){
	Serialize_PackAndSend_TRIPULANTE_TRABAJO(socketCliente, tripulante_id, d_FIN_TAREA);
}

bool Serialize_PackAndSend_SABOTAJE(int socketCliente, uint32_t tipoSabotaje, uint32_t posX, uint32_t posY, uint32_t duracion){
	uint32_t tamMessage = sizeof(uint32_t);
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, tipoSabotaje, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer, posX, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer, posY, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer, duracion, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, d_SABOTAJE);
	free(buffer);
	return resultado;
}

////////////////////////////
// FUNCIONES PARA RECIBIR //
////////////////////////////

HeaderAmongOS_Serialize_RecieveHeader(int socketCliente){
	void* buffer=malloc(sizeof(d_message) + sizeof(uint32_t));
	if(recv(socketCliente, buffer, (sizeof(d_message) + sizeof(uint32_t)), MSG_WAITALL) == 0){
		HeaderAmongOS headerQueRetorna;
		headerQueRetorna.tipoMensaje = (-1);
		headerQueRetorna.tamanioMensaje = 0;
		free(buffer);
		return headerQueRetorna;
	}
	uint32_t tamanioMensaje = 0;
	d_message operacion;
	memcpy(&operacion,buffer,sizeof(d_message));
	memcpy(&tamanioMensaje, buffer+(sizeof(d_message)), (sizeof(uint32_t)));
	free(buffer);
	HeaderAmongOS headerQueRetorna;
	headerQueRetorna.tipoMensaje = operacion;
	headerQueRetorna.tamanioMensaje = tamanioMensaje;
	return headerQueRetorna;
}

void* Serialize_ReceiveAndUnpack(int socketCliente, uint32_t tamanio) {
	void* retorno = malloc(tamanio);
	recv(socketCliente, retorno, tamanio, MSG_WAITALL);
	return retorno;
}

//////////////////////////////////
// FUNCIONES PARA DESEMPAQUETAR //
//////////////////////////////////

uint32_t Serialize_Unpack_ACK(void *pack) {
	uint32_t response = 0;
	memcpy(&response, pack, sizeof(uint32_t));
	return response;
}

uint32_t Serialize_Unpack_idMensaje(void *pack) {
	uint32_t response = 0;
	memcpy(&response, pack, sizeof(uint32_t));
	return response;
}

char* Serialize_Unpack_TareaName(void *buffer) {
	uint32_t tamTarea = 0;
	memcpy(&tamTarea, buffer+sizeof(uint32_t), sizeof(uint32_t));
	char *tarea = malloc(tamTarea);
	memcpy(tarea,buffer,tamTarea);
	return tarea;
}

uint32_t Serialize_Unpack_posX(void *pack) {
	uint32_t response = 0;
	uint32_t tamTarea = 0;
	memcpy(&tamTarea, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + sizeof(uint32_t) + tamTarea , sizeof(uint32_t)); // VERIFICAR QUE SEA ASI CON EL VOID CONST 
	return response;
}

uint32_t Serialize_Unpack_posY(void *pack) {
	uint32_t response = 0;
	uint32_t tamTarea = 0;
	memcpy(&tamTarea, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + sizeof(uint32_t) + tamTarea , sizeof(uint32_t));
	return response;
}

uint32_t Serialize_Unpack_cantidad(void *pack) {
	uint32_t response = 0;
	uint32_t tamTarea = 0;
	memcpy(&tamTarea, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + (2*sizeof(uint32_t)) + tamTarea , sizeof(uint32_t));
	return response;
}

uint32_t Serialize_Unpack_PATOTA_ID(void *pack) {
	uint32_t response = 0;
	uint32_t tamTarea = 0; //no lo queres
	memcpy(&tamTarea, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + tamTarea , sizeof(uint32_t));
	return response; //1
}

uint32_t Serialize_Unpack_TRIPULANTE_ID(void *pack) {
	uint32_t response = 0;
	uint32_t tamTarea = 0;
	memcpy(&tamTarea, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + tamTarea , sizeof(uint32_t));
	return response;
}

char* Serialize_Unpack_TRIPULANTE_ESTADO(void *buffer) {
	char tamTarea = 0;
	memcpy(&tamTarea, buffer, sizeof(char));
	char estado = malloc(tamTarea);
	memcpy(estado,buffer+sizeof(char)+sizeof(uint32_t),tamTarea);
	return estado;
}

uint32_t Serialize_Unpack_posX_TRIPULANTE(void *pack) {
	uint32_t response = 0;
	uint32_t tamTarea = 0;
	memcpy(&tamTarea, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + sizeof(uint32_t) + sizeof(char) + tamTarea , sizeof(uint32_t));
	return response;
}

uint32_t Serialize_Unpack_posY_TRIPULANTE(void *pack) {
	uint32_t response = 0;
	uint32_t tamTarea = 0;
	memcpy(&tamTarea, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + (2*sizeof(uint32_t)) + sizeof(char) + tamTarea , sizeof(uint32_t));
	return response;
}

uint32_t Serialize_Unpack_PROX_TAREA(void *pack) {
	uint32_t response = 0;
	uint32_t tamTarea = 0;
	memcpy(&tamTarea, pack, sizeof(uint32_t));
	memcpy(&response, pack + (3*sizeof(uint32_t)) + sizeof(char) + tamTarea , sizeof(uint32_t));
	return response;
}

uint32_t Serialize_Unpack_PUNTERO_PCB(void *pack) {
	uint32_t response = 0;
	uint32_t tamTarea = 0;
	memcpy(&tamTarea, pack, sizeof(uint32_t));
	memcpy(&response, pack + (4*sizeof(uint32_t)) + sizeof(char) + tamTarea , sizeof(uint32_t));
	return response;
}

char* Serialize_Unpack_TODAS_LAS_TAREAS(void *buffer) {
	uint32_t tamTarea = 0;
	memcpy(&tamTarea, buffer+sizeof(uint32_t), sizeof(uint32_t));
	char *tarea = malloc(tamTarea);
	memcpy(tarea,buffer+sizeof(uint32_t),tamTarea);
	return tarea;
}

uint32_t Serialize_Unpack_NUEVA_POS_TRIPULANTE(void *pack) {
	uint32_t response = 0;
	uint32_t tamTarea = 0; //TAMTAREA ES EL TAMANIO DEL DATO
	memcpy(&tamTarea, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + sizeof(uint32_t) + tamTarea, sizeof(uint32_t));
	return response;
}

uint32_t Serialize_Unpack_VIEJA_POS_TRIPULANTE(void *pack) {
	uint32_t response = 0;
	uint32_t tamTarea = 0; //TAMTAREA ES EL TAMANIO DEL DATO
	memcpy(&tamTarea, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + (2*sizeof(uint32_t)) + tamTarea, sizeof(uint32_t));
	return response;
}

char* Serialize_Unpack_TRIPULANTE_ESTADO(void *buffer) {
	char tamTarea = 0;
	memcpy(&tamTarea, buffer, sizeof(char));
	char estado = malloc(tamTarea);
	memcpy(estado,buffer+sizeof(uint32_t),tamTarea);
	return estado;
}

char* Serialize_Unpack_DAME_TRIPULANTES(void *buffer) {
	char tamTarea = 0;
	memcpy(&tamTarea, buffer, sizeof(char));
	char estado = malloc(tamTarea);
	memcpy(estado,buffer,tamTarea);
	return estado;
}

char* Serialize_Unpack_TIPO_SABOTAJE(void *buffer) {
	uint32_t response = 0;
	uint32_t tamTarea = 0; //TAMTAREA ES EL TAMANIO DEL DATO
	memcpy(&tamTarea, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + tamTarea, sizeof(uint32_t));
	return response;
}

char* Serialize_Unpack_POSX_SABOTAJE(void *buffer) {
	uint32_t response = 0;
	uint32_t tamTarea = 0; //TAMTAREA ES EL TAMANIO DEL DATO
	memcpy(&tamTarea, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + sizeof(uint32_t) + tamTarea, sizeof(uint32_t));
	return response;
}

char* Serialize_Unpack_POSY_SABOTAJE(void *buffer) {
	uint32_t response = 0;
	uint32_t tamTarea = 0; //TAMTAREA ES EL TAMANIO DEL DATO
	memcpy(&tamTarea, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + (2*sizeof(uint32_t)) + tamTarea, sizeof(uint32_t));
	return response;


char* Serialize_Unpack_DURACION_SABOTAJE(void *buffer) {
	uint32_t response = 0;
	uint32_t tamTarea = 0; //TAMTAREA ES EL TAMANIO DEL DATO
	memcpy(&tamTarea, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + (3*sizeof(uint32_t)) + tamTarea, sizeof(uint32_t));
	return response;
}

//////////////////////////////////////
// FUNCIONES PARA DESEMPAQUETAR PRO //
/////////////////////////////////////

void Serialize_Unpack_TAREA(void *packTarea, uint32_t *idMensaje, char *nombre, uint32_t *posX, uint32_t *posY, uint32_t *cantidad){
	*idMensaje = Serialize_Unpack_idMensaje(packTarea);
	nombre = Serialize_Unpack_TareaName(packTarea);
	posX = Serialize_Unpack_posX(packTarea);
	posY = Serialize_Unpack_posY(packTarea);
	cantidad = Serialize_Unpack_cantidad(packTarea);
}

void Serialize_Unpack_PATOTA(void *packPatotaNew, uint32_t *idMensaje, uint32_t patota_id, char *todasLasTareas){
	*idMensaje = Serialize_Unpack_idMensaje(packPatotaNew);
	patota_id = Serialize_Unpack_idMensaje(packPatotaNew);
	todasLasTareas =  Serialize_Unpack_TRIPULANTE_ID(packPatotaNew);
}

void Serialize_Unpack_TRIPULANTE(void *packTripulante, uint32_t *idMensaje, uint32_t tripulante_id, char estado, uint32_t posX, uint32_t posY, uint32_t prox_tarea, uint32_t puntero_PCB){
	*idMensaje = Serialize_Unpack_idMensaje(packTripulante);
	tripulante_id =  Serialize_Unpack_TRIPULANTE_ID(packTripulante);
	estado = Serialize_Unpack_TRIPULANTE_ESTADO(packTripulante);
	posX = Serialize_Unpack_posX(packTripulante);
	posY = Serialize_Unpack_posY(packTripulante);
	prox_tarea = Serialize_Unpack_PROX_TAREA(packTripulante);
	puntero_PCB = Serialize_Unpack_PUNTERO_PCB(packTripulante);
}

void Serialize_Unpack_BITACORAoELIMINAR(void *packTripulante, uint32_t *idMensaje, uint32_t tripulante_id){
	*idMensaje = Serialize_Unpack_idMensaje(packTripulante);
	tripulante_id =  Serialize_Unpack_TRIPULANTE_ID(packTripulante);
}

void Serialize_Unpack_TRIPULANTE_POS_NUEVA_X(void *packTripulante, uint32_t *idMensaje, uint32_t tripulante_id, uint32_t posX, uint32_t viejaPosX){
	*idMensaje = Serialize_Unpack_idMensaje(packTripulante);
	tripulante_id =  Serialize_Unpack_TRIPULANTE_ID(packTripulante);
	posX = Serialize_Unpack_NUEVA_POS_TRIPULANTE(packTripulante);
	viejaPosX = Serialize_Unpack_VIEJA_POS_TRIPULANTE(packTripulante);
}

void Serialize_Unpack_TRIPULANTE_POS_NUEVA_Y(void *packTripulante, uint32_t *idMensaje, uint32_t tripulante_id, uint32_t posY, uint32_t viejaPosY){
	*idMensaje = Serialize_Unpack_idMensaje(packTripulante);
	tripulante_id =  Serialize_Unpack_TRIPULANTE_ID(packTripulante);
	posY = Serialize_Unpack_NUEVA_POS_TRIPULANTE(packTripulante);
	viejaPosY = Serialize_Unpack_VIEJA_POS_TRIPULANTE(packTripulante);
}

void Serialize_Unpack_TRIPULANTE_ESTADO_NUEVO(void *packTripulante, uint32_t *idMensaje, uint32_t tripulante_id, char estado){
	*idMensaje = Serialize_Unpack_idMensaje(packTripulante);
	tripulante_id =  Serialize_Unpack_TRIPULANTE_ID(packTripulante);
	estado = Serialize_Unpack_TRIPULANTE_ESTADO(packTripulante);
}

void Serialize_Unpack_DAME_TRIPULANTE(void *packTripulante, uint32_t *idMensaje, char estado){
	*idMensaje = Serialize_Unpack_idMensaje(packTripulante);
	estado = Serialize_Unpack_DAME_TRIPULANTES(packTripulante);
}

void Serialize_Unpack_TRIPULANTE_INICIO_TAREA(void *packTripulante, uint32_t *idMensaje, uint32_t tripulante_id, uint32_t posX, uint32_t viejaPosX){
	*idMensaje = Serialize_Unpack_idMensaje(packTripulante);
	tripulante_id =  Serialize_Unpack_TRIPULANTE_ID(packTripulante);
}

void Serialize_Unpack_TRIPULANTE_FIN_TAREA(void *packTripulante, uint32_t *idMensaje, uint32_t tripulante_id){
	*idMensaje = Serialize_Unpack_idMensaje(packTripulante);
	tripulante_id =  Serialize_Unpack_TRIPULANTE_ID(packTripulante);
}

void Serialize_Unpack_SABOTAJE(void *packTripulante, uint32_t *idMensaje, uint32_t tipoSabotaje, uint32_t posX, uint32_t posY, uint32_t duracion){
	*idMensaje = Serialize_Unpack_idMensaje(packTripulante);
	tipoSabotaje =  Serialize_Unpack_TIPO_SABOTAJE(packTripulante);
	posX = Serialize_Unpack_POSX_SABOTAJE(packTripulante);
	posY = Serialize_Unpack_POSY_SABOTAJE(packTripulante);
	duracion = Serialize_Unpack_DURACION_SABOTAJ(packTripulante);
}
