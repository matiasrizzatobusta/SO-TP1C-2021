#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

int main(int argc, char **argv) {

	int conexion_mongoStore;
	int conexion_miRam;
	char* ip_miRam;
	char* ip_iMongo;
	char* puerto_miRam;
	char* puerto_iMongo;
	char* valor;

	t_config* config;
	t_log* logger;

	//traigo los puertos para luego hacer la conexion
	config = leer_config();
	ip_miRam = config_get_string_value(config, "IP_MI_RAM_HQ");
	ip_iMongo = config_get_string_value(config, "IP_I_MONGO_STORE");

	puerto_miRam = config_get_string_value(config,"PUERTO_MI_RAM_HQ");
	puerto_iMongo =config_get_string_value(config,"PUERTO_I_MONGO_STORE");

	//CONECTO CON EL I_MONGO_SOTRE
	conexion_mongoStore = crear_conexion(ip_iMongo,puerto_iMongo);
	conexion_miRam = crear_conexion(ip_miRam,puerto_miRam);

	/*if (conexion_mongoStore < 0){
			error_show("No se conecto con el servidor I-Mongo-Store\n");
	}

	if(conexion_miRam < 0){
		error_show("No se conecto con el servidor Mi-Ram\n");
	}
	/*






}
9
int main(void) {

	return(0);

}
