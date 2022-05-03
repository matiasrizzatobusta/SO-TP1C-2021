#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

// MODULO DISCORDIADOR

// CREAR 3 PAQUETES: OBTENER_TAREA , GRABAR_BITACORA , EJECUTAR_TAREA son muchas tareas o puede ser solo una?
// CREAR 3 FUNCIONES DE SERIALIZACION DIISTINTA PARA CADA PAQUETE

typedef struct { //CONFIRMAR QUE EL PAQUETE TIENE UN TAMANIO FIJO Y QUE ESE TAMNIO ES EL DEFINIDO
	char nombreTarea[17]; //lo mas largo puede ser generar_Oxigeno 16char {g,e,n,e,r,a,r,_,O,x,i,g,e,n,o}
	uint8_t parametroTarea; //un numero entre 0 y 127
}tarea; //es el mismo paquete para obetener y para enviar

typedef struct {
	//VER QUE CARAJO VA ACA ADENTRO en principio un mega string
	char *queHizoElTripulante;
}bitacora;


int main(int argc, char **argv) {

	int conexion_miRam;
	int conexion_iMongo;
	char* ip_miRam;
	char* ip_iMongo;
	char* puerto_miRam;
	char* puerto_iMongo;
	char* valor_miRam;
	char* valor_iMongo;

	t_config* config;
	//t_config* conexiones_Config;
	t_log* logger;

	//traigo los puertos para luego hacer la conexion
	conectar_Miram();
	conectar_iMongoStore();

	config = leer_config(); // hacer leer config

	logger = log_create()

}

void conectar_MiRam(){
	ip_miRam = config_get_string_value(tpConfig, "IP_MI_RAM_HQ");
	puerto_miRam = config_get_string_value(tpConfig,"PUERTO_MI_RAM_HQ");

	conexion_MiRam = crear_conexion(ip_miRam, puerto_miRam);

	if(conexion_miRam < 0){
		socket_close_conection(conexion_MiRam);
		error_show("Error al establecer conexion con el servidor Mi-Ram");
	}else{ 
	// hacer lo que se tiene que hacer al conectar con el servidor 
	}

}

void conectar_IMongoStore(){
	config = leer_config();
	ip_IMongo = config_get_string_value(tpConfig, "IP_I_MONGO_STORE");
	puerto_IMongo = (tpConfig,'PUERTO_I_MONGO_STORE')

	conexion_MiRam = crear_conexion(ip_iMongo, puerto_IMongo);

	if(conexion_iMongo < 0){
		socket_close_conection(conexion_iMongo);
		error_show("Error al establecer conexion con el servidor I-Mongo-Store");
	}else{ 
	// hacer lo que se tiene que hacer al conectar con el servidor 
	}
	
}