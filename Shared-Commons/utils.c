#include "utils.h"

//terminar de entender como funciona todo esto xd

static int NEW_LINE = '\n';
static int END_LINE = '\0';
static int DOT = '.';
static int SLIDE = '/';
static char* COMMA = ",";
static char* SPACE = " ";
static char* EMPTY_STRING = "";
static char* OPENING_SQUARE_BRACKET = "[";
static char* CLOSING_SQUARE_BRACKET = "]";


void utils_end_string(char *string) {
	if ((strlen(string) > 0) && (string[strlen(string) - 1] == NEW_LINE))
		string[strlen(string) - 1] = END_LINE;
}

bool utils_is_empty(char* string) {
	return string == NULL || string_is_empty(string);
}

void utils_delay(int seconds) {
	sleep(seconds);
}

void buffer_create(t_paquete* paquete){
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_package* package_create(t_protocol code){ // aca estamos enviando por parametro el codigo de mensaje que esdtamos enviando,
                                            // 1 = tarea, 2 = bitacora PUEDEN FALTAR MAS
	t_package* package = malloc(sizeof(t_package));
	package->operation_code = code;
	utils_buffer_create(package);
	return package;
}

void package_add(t_package* package, void* value, int size){
	package->buffer->stream = realloc(package->buffer->stream,
			package->buffer->size + size + sizeof(int));

	memcpy(package->buffer->stream + package->buffer->size, &size, sizeof(int));
	memcpy(package->buffer->stream + package->buffer->size + sizeof(int), value,
			size);

	package->buffer->size += size + sizeof(int);
}

void package_destroy(t_package* package) {
	free(package->buffer->stream);
	free(package->buffer);
	free(package);
}

void package_send_to(t_package* t_package, int client_socket) {
	int bytes = t_package->buffer->size + 2 * sizeof(int);
	void* to_send = serializer_serialize_package(t_package, bytes);
	send(client_socket, to_send, bytes, 0);

	free(to_send);
}

void destroy_list(t_buffer *self) {
	free(self->stream);
	free(self);
}

void get_from_list_to_malloc(void *parameter, t_list *list, int index) {
	t_buffer *buffer;
	buffer = list_get(list, index);
	void *pointer = malloc(buffer->size);
	memcpy(pointer, buffer->stream, buffer->size);
	parameter = pointer;
}

int get_buffer_size(t_list *list, int index) {
	if (list_size(list) > 0) {
		t_buffer *buffer;
		buffer = list_get(list, index);
		return buffer->size;
	}
	return 0;
}

t_list* utils_receive_package(int socket_cliente) {
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio = 0;

	buffer = utils_receive_buffer(&size, socket_cliente);

	while (desplazamiento < size) {
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		t_buffer* valor = malloc(sizeof(t_buffer));
		valor->stream = malloc(tamanio);
		valor->size = tamanio;
		memcpy(valor->stream, buffer + desplazamiento, tamanio);
		desplazamiento += tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
	return NULL;
}

/*
void utils_serialize_and_send(int socket, int protocol, void* package_send) {
	switch (protocol) {

	case HANDSHAKE: {
		break;
	}

	case NEW_POKEMON: {
		t_package* package = utils_package_create(protocol);
		utils_package_add(package,
				&((t_new_pokemon*) package_send)->tamanio_nombre,
				sizeof(uint32_t));
		utils_package_add(package,
				((t_new_pokemon*) package_send)->nombre_pokemon,
				strlen(((t_new_pokemon*) package_send)->nombre_pokemon) + 1);
		utils_package_add(package,
				&((t_new_pokemon*) package_send)->id_correlacional,
				sizeof(uint32_t));
		utils_package_add(package, &((t_new_pokemon*) package_send)->cantidad,
				sizeof(uint32_t));
		utils_package_add(package, &((t_new_pokemon*) package_send)->pos_x,
				sizeof(uint32_t));
		utils_package_add(package, &((t_new_pokemon*) package_send)->pos_y,
				sizeof(uint32_t));
		utils_package_send_to(package, socket);
		utils_package_destroy(package);
		break;
	}

	}
}

void* utils_receive_and_deserialize(int socket, int package_type) {
	void iterator(t_buffer* value) {
		printf("%d \n", value->size);
		int dest;
		memcpy(&dest, value->stream, value->size);
		printf("%d \n", dest);
	}
	switch (package_type) {

	case NEW_TAREA: {
		t_new_tarea *new_request = malloc(sizeof(t_new_tarea));
		t_list* list = utils_receive_package(socket);
		utils_get_from_list_to(&new_request->tamanio_nombre, list, 0);
		new_request->idTripulante = malloc(utils_get_buffer_size(list, 1));
		utils_get_from_list_to(new_request->idTripulante, list, 1);
		utils_get_from_list_to(&new_request->idPatota, list, 2);
		utils_get_from_list_to(&new_request->pos_x, list, 3);
		utils_get_from_list_to(&new_request->pos_y, list, 4);
		list_destroy_and_destroy_elements(list, (void*) utils_destroy_list);
		return new_request;
	}

	}
	return NULL;
}
*/

void utils_get_from_list_to(void *parameter, t_list *list, int index) {
	t_buffer *buffer;
	buffer = list_get(list, index);
	memcpy(parameter, buffer->stream, buffer->size);
}