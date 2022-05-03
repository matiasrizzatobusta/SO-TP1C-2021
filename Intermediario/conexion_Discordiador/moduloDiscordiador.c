#include "discordiador.h"

int main(int argc, char *argv[]) {
    
}

int discordiador_cargar(){
	int respuesta = discordiador_config_carga();

	if(respuesta < 0){
		discordiador_config_free();
		return respuesta;
	}

	discordiador_print_config();

	return 0;
}

void discoridador_server_init(){//parecido a socket cliente del tp0,solo que este es de escucha

	discordiador_socket = socket_create_listener(discordiador_config->ip_discordiador,discordiador_config->puerto_discordiador); 
	
	if(broker_socket < 0)
	{
		broker_logger_error("Error al crear server");
		return;
	}
	//msg que se imprime si encuentra una conexion
	discoridador_logger_info("Server creado correctamente!! Esperando conexiones...");

	//terminar de entender como funciona lo del accepted y eso(y ver donde van los logger ingo y eso xdxd)
	if((accepted_fd = accept(discoridador_socket, (struct sockaddr *) &client_info, &addrlen)) != -1)
		{
			pthread_create(&tid, NULL, (void*) handle_connection, (void*) &accepted_fd);
			pthread_detach(tid);
			discordiador_logger_info("Creando un hilo para atender una conexión en el socket %d", accepted_fd);
		}
		else
		{
			discordiador_logger_error("Error al conectar con un cliente");
		}
}

static void *handle_connection(void *arg){ //para recibir msgs

	int client_fd = *((int *) arg);

	discordiador_logger_info("Conexion establecida con cliente: %d", client_fd);
	int received_bytes;
	int protocol;

	while(true){
		received_bytes = recv(client_fd, &protocol, sizeof(int), 0);

		if(received_bytes <= 0){
			discordiador_logger_error("Error al recibir mensaje");
			return NULL;
		}
		switch(protocol){ //ver como funciona un handshake
			case HANDSHAKE:
				broker_logger_info("Recibi una nueva conexion");
				break;
			default:
				break;
		}
	}
}

void finalizar_conexion(){
	socket_close_conection(discoridador_socket);
	broker_config_free();
	broker_logger_destroy();
}
//falta hacer la discordiador_config.c