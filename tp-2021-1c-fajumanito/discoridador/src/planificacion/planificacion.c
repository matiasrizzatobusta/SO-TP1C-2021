#include "planificacion.h"

int tripulantes_totales = 0;
char* split_char = "|";

int main(int argc, char *argv[]){
	inicializar(argc,argv);
	return EXIT_SUCCESS;
}

void inicializar(int argc, char *argv[]){
	discordiador_log = crear_log("Discordiador");
	inicializar_semaforos();
	SEGUIR_ATENDIENDO = true;
	HAY_SABOTAJE = false; // si esto esta en true, se corta la planif y everything lo demas
	inicializarConfig(argc,argv);
	//iniciar_patota();
	TAREAS_PENDIENTES = list_create();
	log_info (discordiador_log, "PATOTA OK");
	crear_hilo_planificacion();

	servidor = malloc(sizeof(pthread_t));
	iniciarServidorMIRAMHQ(servidor);

	pthread_join(*servidor, NULL);
}

int iniciarConexionAMIRAMHQ(){
	int conexion;
	while (1) {
		sem_wait(&semaforoConexionAMIRAMHQ);
		conexion = conectarse_a_un_servidor(tpConfig->IP_MI_RAM_HQ, tpConfig->PUERTO_MI_RAM_HQ, discordiador_log);//a agregar
		sem_post(&semaforoConexionAMIRAMHQ);
		if (conexion == -1) {
			log_error(discordiador_log,
					"No se pudo conectar con MI RAM HQ");
		} else {
			break;
		}
	}
	return conexion;
}

uint32_t recibirResponse(int conexion, HeaderAmongOS headerACK){
	if(headerACK.tipoMensaje == d_ACK)
	{
		void* packACK = Serialize_ReceiveAndUnpack(conexion, headerACK.tamanioMensaje);
		uint32_t id = Serialize_Unpack_ACK(packACK);
		free(packACK);
		//cerrar conexion;
		close(conexion);
		return id;
	}
	else
		return -1; //codigo de error
}

void iniciarServidorMIRAMHQ(pthread_t* servidor) {
	if (pthread_create(servidor, NULL, (void*) atenderMIRAMHQ, NULL) == 0) {
		log_info(discordiador_log, "::::Se creo hilo de MI RAM HQ::::");
	} else {
		log_error(discordiador_log, "::::No se pudo crear el hilo de MI RAM HQ::::");
	}
}

void* atenderMIRAMHQ(){
	sem_t semaforoSocket;
	int conexion;
	sem_init(&semaforoSocket,0,1);
	t_log* miRamHqLog = crear_log("MIRAMHQ");
	conexion = iniciar_servidor(tpConfig ->IP_MI_RAM_HQ, tpConfig->PUERTO_MI_RAM_HQ ,miRamHqLog); //falta agregar
	while(SEGUIR_ATENDIENDO) {
		int cliente = esperar_cliente_con_accept(conexion, miRamHqLog); //falta agregar
		sem_wait(&semaforoMIRAMHQ);
		log_info(miRamHqLog, "se conecto cliente: %i", cliente);
		pthread_t* dondeSeAtiende = malloc(sizeof(pthread_t));
		sem_wait(&semaforoSocket);
		p_elementoDeHilo *elemento = malloc(sizeof(p_elementoDeHilo));
		elemento->log =miRamHqLog;
		elemento->cliente = cliente;
		sem_post(&semaforoSocket);

		if (pthread_create(dondeSeAtiende, NULL,
				(void*) recibirYAtenderUnClienteMIRAMHQ, elemento) == 0) {
			log_info(miRamHqLog, ":::: Se creo hilo para cliente ::::");
		} else {
			log_error(miRamHqLog,":::: No se pudo crear el hilo para cliente ::::");

		}
		pthread_detach(*dondeSeAtiende);
		sem_post(&semaforoMIRAMHQ);
	}
	return NULL;
}
void* recibirYAtenderUnClienteMIRAMHQ(p_elementoDeHilo* elemento) {
	while (SEGUIR_ATENDIENDO) {
		sem_wait(&semaforoSocketMIRAMHQ);
		HeaderAmongOS headerRecibido = Serialize_RecieveHeader(elemento->cliente);
		if (headerRecibido.tipoMensaje == -1) {
			log_error(elemento->log, "Se desconecto el cliente\n");
			sem_post(&semaforoSocketMIRAMHQ);
			break;
		}
		atender(headerRecibido, elemento->cliente, elemento->log); //a agregar
		sem_post(&semaforoSocketMIRAMHQ);
	}
	free(elemento);
	return 0;
}

void* recibirYAtenderUnCliente(p_elementoDeHilo* elemento) {
	sem_t semaforoSocket;
	sem_init(&semaforoSocket,0,1);
	while (SEGUIR_ATENDIENDO) {
		sem_wait(&semaforoSocket);
		HeaderAmongOS headerRecibido = Serialize_RecieveHeader(elemento->cliente);
		if (headerRecibido.tipoMensaje == -1) {
			log_error(elemento->log, "Se desconecto el cliente\n");
			sem_post(&semaforoSocket);
			break;
		}
		atender(headerRecibido, elemento->cliente, elemento->log);
		sem_post(&semaforoSocket);
	}
	sem_destroy(&semaforoSocket);
	free(elemento);
	return 0;
}
// funcion atender(HeaderAmongOS header, int cliente, t_log* logger) ya existe?
void destruirTarea(t_tarea *tarea){
	free(tarea);
}

void finalizar(){
	sleep(tpConfig->RETARDO_CICLO_CPU);
	//logearFin();
	//destruirTodo();
}

void inicializarConfig(int argc, char *argv[]){
	if(argc == 2){
		creacionConfig = config_create(argv[1]);
	}else{
		creacionConfig = config_create("../tpConfig.config");
	}

    tiempo_retardo = config_get_int_value(creacionConfig, "RETARDO_CICLO_CPU");
    grado_multiprocesamiento = config_get_int_value(creacionConfig, "GRADO_DE_MULTIPROCESAMIENTO");
    algoritmo = config_get_string_value(creacionConfig, "ALGORITMO_DE_PLANIFICACION");
    quantum = config_get_int_value (creacionConfig, "QUANTUM");
    tripulantes_trabajando = 0;

	tpConfig->IP_MI_RAM_HQ = config_get_string_value(creacionConfig, "IP_MI_RAM_HQ");
	tpConfig->PUERTO_MI_RAM_HQ = config_get_int_value(creacionConfig, "PUERTO_MI_RAM_HQ");
	tpConfig->IP_I_MONGO_STORE = config_get_string_value(creacionConfig, "IP_I_MONGO_STORE");
	tpConfig->PUERTO_I_MONGO_STORE = config_get_int_value(creacionConfig, "PUERTO_I_MONGO_STORE");
    tpConfig->GRADO_MULTITAREA = config_get_int_value(creacionConfig, "GRADO_MULTITAREA");
    tpConfig->ALGORITMO = config_get_string_value(creacionConfig, "ALGORITMO");
    tpConfig->QUANTUM = config_get_int_value(creacionConfig, "QUANTUM");
    tpConfig->DURACION_SABOTAJE = config_get_int_value(creacionConfig, "DURACION_SABOTAJE");
	tpConfig->RETARDO_CICLO_CPU = config_get_int_value(creacionConfig, "RETARDO_CICLO_CPU");
}

void crear_semaforos(){
	sem_init(&semaforoSocketMIRAMHQ,0,0);
	sem_init(&semaforoMIRAMHQ,0,0);
	sem_init(&semaforoSocket,0,0);
	sem_init(&semaforoCambioDeEstado,0,0);
	sem_init(&mutex_FIFO,0,0);
	sem_init(&mutex_RR,0,0);
	sem_init(&mutex_ready,0,0);
	sem_init(&mutex_exit,0,0);
	sem_init(&mutex_blocked_sabotaje,0,0);
}

void crear_hilo_planificacion(){
	pthread_create(&hiloPlanificacion, NULL, (void*)planificar_segun, tpConfig->ALGORITMO);
	pthread_detach(hiloPlanificacion);
}

void enviar_ready(t_tripulante *tripulante){
    sem_wait(&mutex_ready);
    list_add(listaReady, tripulante);
    sem_post(&mutex_ready);
    if (!strcmp(algoritmo, "FIFO"))
        sem_post(&mutex_FIFO);
    if (!strcmp(algoritmo, "RR"))
        sem_post(&mutex_RR);
}

void *pasar_a_block(void *proceso){
    t_tripulante *tripulante = (t_tripulante *)proceso;
    t_tarea tareaTripu = tripulante->tareaActual;
    for (int i = 0; i < tareaTripu.ciclos_de_cpu; i++)
    {
        //log_info(logger, "Tripulante %c en periodo de descanso", tripulante->id);
    }
    enviar_ready(tripulante);

    return 0;
}

void planificar_segun(char* tipoPlanif){

	while(!HAY_SABOTAJE){
		if(string_equals_ignore_case(tipoPlanif,"FIFO")){
			ALGORITMO_FIFO();
			return;
		}
		if(string_equals_ignore_case(tipoPlanif,"RR")){
			algoritmo_RR();
			return;
		}else{
			 error_show("NO SE RECONOCE EL ALGORITMO DE PLANIFICACION\n");
			return;
		}
	}
}

void desplazar_tripulante(t_tripulante *tripulante){
	int posVieja;
    if (tripulante->posX < tripulante->tareaActual.posX){
		posVieja = tripulante->posX;
        tripulante->posX++;
		Serialize_PackAndSend_TRIPULANTE_NEW_POSX(tpConfig->PUERTO_MI_RAM_HQ , tripulante->id, tripulante->posX, posVieja);
		Serialize_PackAndSend_TRIPULANTE_NEW_POSX(tpConfig->PUERTO_I_MONGO_STORE, tripulante->id, tripulante->posX, posVieja);
	}
    else if (tripulante->posX > tripulante->tareaActual.posX){
		posVieja = tripulante->posX;
        tripulante->posX--;
		Serialize_PackAndSend_TRIPULANTE_NEW_POSX(tpConfig->PUERTO_MI_RAM_HQ , tripulante->id, tripulante->posX, posVieja);
		Serialize_PackAndSend_TRIPULANTE_NEW_POSX(tpConfig->PUERTO_I_MONGO_STORE, tripulante->id, tripulante->posX, posVieja);
	}
    else{
        if (tripulante->posY < tripulante->tareaActual.posY){
			posVieja = tripulante->posY;
        	tripulante->posY--;
			Serialize_PackAndSend_TRIPULANTE_NEW_POSX(tpConfig->PUERTO_MI_RAM_HQ, tripulante->id, tripulante->posY, posVieja);
			Serialize_PackAndSend_TRIPULANTE_NEW_POSX(tpConfig->PUERTO_I_MONGO_STORE, tripulante->id, tripulante->posY, posVieja);
		}
        else if (tripulante->posY > tripulante->tareaActual.posY){
			posVieja = tripulante->posY;
        	tripulante->posY--;
			Serialize_PackAndSend_TRIPULANTE_NEW_POSX(tpConfig->PUERTO_MI_RAM_HQ, tripulante->id, tripulante->posY, posVieja);
			Serialize_PackAndSend_TRIPULANTE_NEW_POSX(tpConfig->PUERTO_I_MONGO_STORE, tripulante->id, tripulante->posY, posVieja);
		}

    }
	tripulante->ciclos_cpu_ejecutados++;
}

/////////////////////////////////
// ALGORITMOS DE PLANIFICACION //
/////////////////////////////////

bool llegoATarea(t_tripulante *tripulante){
	return tripulante->posX == tripulante->tareaActual.posX && tripulante->posY == tripulante->tareaActual.posY;
}

t_tripulante* discordiador_exec_tripulante(t_tripulante* tripulante){
	tripulante->estado = 'e';
	if(tripulante->tareaActual.nombre == NULL){
		//mandar tripulante a finish -> que finish termine al tripulante todo
	}

	if(llegoATarea(tripulante)){

		switch (atoi(tripulante->tareaActual.nombre)){

		case generar_oxigeno:;
			generar_Oxigeno(tripulante->tareaActual.ciclos_de_cpu);
			tripulante->ciclos_cpu_ejecutados++;
			break;
		case consumir_oxigeno:;
			consumir_Oxigeno(tripulante->tareaActual.ciclos_de_cpu);
			tripulante->ciclos_cpu_ejecutados++;
			break;
		case generar_comida:;
			generar_Comida(tripulante->tareaActual.ciclos_de_cpu);
			tripulante->ciclos_cpu_ejecutados++;
			break;
		case consumir_comida:;
			consumir_Comida(tripulante->tareaActual.ciclos_de_cpu);
			tripulante->ciclos_cpu_ejecutados++;
			break;
		case generar_basura:;
			generar_Basura(tripulante->tareaActual.ciclos_de_cpu);
			tripulante->ciclos_cpu_ejecutados++;
			break;
		case descartar_basura:; //porque no lo toma??
			consumir_Comida(tripulante->tareaActual.ciclos_de_cpu);
			tripulante->ciclos_cpu_ejecutados++;
			break;
		default:
			//tareaRandomPorConsola
			break;
		}
	}else{
		desplazar_tripulante(tripulante);
	}

	return tripulante;
}
//Para RR
void sumarCiclos(t_tripulante *tripulante, int ciclos){
	tripulante->ciclos_cpu_ejecutados = tripulante->ciclos_cpu_ejecutados + ciclos;
	sleep(tpConfig->RETARDO_CICLO_CPU);
	if(string_equals_ignore_case(tpConfig->ALGORITMO, "RR")){
		sem_post(&semaforoPlanifiquenme);
		sem_wait(&(tripulante->semaforoDeTripulante));
	}
}
//para RR y FIFO
void avisoTareaTerminada(t_tripulante *tripulante){
	tripulante->ciclos_cpu_ejecutados = 0;
	if(string_equals_ignore_case(tpConfig->ALGORITMO, "RR") || string_equals_ignore_case(tpConfig->ALGORITMO, "FIFO")){
		sem_post(&semaforoPlanifiquenme);
	}
	else{
		sem_post(&semaforoTermine);
	}
}

void ALGORITMO_FIFO(){
	while(tripulantes_totales <= tpConfig->GRADO_MULTITAREA){
	sem_init(&semaforoTermine,0,0);
    while (1)
    {
    	sem_wait(&semaforoCambioDeEstado);
    	t_tripulante *tripulante = list_get(listaReady,0);
    	sem_post(&semaforoCambioDeEstado);
    	log_info(discordiador_log, "Se planificara al tripulante de id: %i",tripulante->id);
    	sem_post(&(tripulante->semaforoDeTripulante));
    	sem_wait(&semaforoTermine);
    	log_info(discordiador_log, "Se va a intercambiar al tripulante de id: %i porque termino sus rafagas", tripulante->id);
		}
		tripulantes_totales++;
	}
	tripulantes_totales++;
}




//para RR
void ponerAlFinalDeLista(t_tripulante *tripulante, t_list *lista){
	sem_wait(&semaforoCambioDeEstado);
	int index = list_get_index(lista,tripulante, (void*)llegoATarea);
	if(index != -1){
		list_remove(lista,index);
		list_add(lista,tripulante);
	}
	tripulante->ciclos_cpu_ejecutados = 0;
	sem_post(&semaforoCambioDeEstado);
}

void algoritmo_RR(){
	while(tripulantes_totales <= tpConfig->GRADO_MULTITAREA){
		sem_init(&semaforoPlanifiquenme,0,0);
		t_tripulante *unTripulante;
		sem_wait(&semaforoCambioDeEstado);
		unTripulante = list_get(listaReady,0);
		sem_post(&semaforoCambioDeEstado);
		log_info(discordiador_log, "\n ::: Se planificara al tripulante de ID: %i ::: \n",unTripulante->id);
		sem_post(&(unTripulante->semaforoDeTripulante));

		while(1){
			sem_wait(&semaforoPlanifiquenme);
			if(unTripulante->tareaActual.nombre == NULL){
				log_info(discordiador_log, "Se va a intercambiar al tripulante de ID: %i porque termino su tarea", unTripulante->id);
				break;
			}else if(unTripulante->ciclos_cpu_ejecutados < tpConfig->QUANTUM){
				sem_post(&(unTripulante->semaforoDeTripulante));
			}else{
				log_info(discordiador_log, "Se va a intercambiar al tripulante: %i por fin de quantum", unTripulante->id);
				ponerAlFinalDeLista(unTripulante,listaReady);
				break;
			}
		}
		tripulantes_totales++;
	}
}

bool noHayMasTareasTripulante(t_tripulante *tripulante){
	if (tripulante->tareaActual.nombre == NULL){
		return true;
	}else{
		return false;
	}
}

bool _ordenarPorId(void* element1,void* element2){
	t_tripulante* tripulanteA = (t_tripulante*)element1;
	t_tripulante* tripulanteB = (t_tripulante*)element2;

	if(tripulanteA->id < tripulanteB->id){
		return true;
	}else{
		return false;
	}
}

//////////////
// CONSOLA  //
//////////////

int id_patota = 1, id_tripulante = 1;//pasar a archi con var globales

void iniciar_patota(char *iniciacion){ 
	int cantTripulantes;
	int posX;
	int posY;
	char *todas_las_tareas_string;

	char *iniciar = iniciacion;
	const char delim_espacio[2] = " "; //para separar el choclo que entra por espacios
	const char delim_pipe[2] = "|";	//para separar la pos que entra
	char *ptr = strtok(iniciar, delim_espacio); //puntero temporal al string entro que me llego cuando llamo a la funcion

	cantTripulantes = atoi( ptr);
	char asigno_estado_new = 'n';

	ptr = strtok(iniciar, delim_espacio); //la llamo de vuelta para leer el siguiente pedazo de texto
	FILE *archi;
	if (ptr){
		archi = fopen(ptr, "r");
  		fseek (archi, 0, SEEK_END);
  		length = ftell (archi);
  		fseek (archi, 0, SEEK_SET);

  		if (todas_las_tareas_string){
    		fread (todas_las_tareas_string, 1, length, archi);
			Serialize_PackAndSend_NEW_PATOTA(tpConfig-> PUERTO_MI_RAM_HQ, id_patota, todas_las_tareas_string);
  		}
	}else{
		printf("El archivo %s ingresado no existe", ptr);
		return;
	}
	fclose (archi);
	ptr = strtok (iniciar, delim_espacio); //aca estoy parado en la primer posicion
    for(int i = 0; i <= cantTripulantes; i++){
		while (ptr != NULL){	//en el caso de que haya mas pos que trip no me importa xq no lo voy a llegar a leer
			char *posXY = ptr;
			posX = atoi(strtok (posXY, delim_pipe));
			posY = atoi(strtok (posXY, delim_espacio));
			iniciar_tripulante(id_tripulante, asigno_estado_new, posX, posY,id_patota); //puse estado new xq si o si son estado new
			ptr = strtok (iniciar, delim_espacio);
		}
		posX = 0; //no me importa que esto este aca xq si ptr es null no hay nada mas que leer, osea no hay mas
		posY = 0; //posiciones para darle a los tripulantes
		iniciar_tripulante(id_tripulante, asigno_estado_new, posX, posY,id_patota);
        id_tripulante++;
    }
    id_patota++;	//guardo el numero de patota cargada originalmente
                	//ojo q aca se esta guaradando el de la siguiente no el de la original

}

void iniciar_tripulante(int id_tripulante, char estado_tripulante, int posX, int posY, int id_patota){
	uint32_t tareaVacia, punteroVacio = 0;
	Serialize_PackAndSend_TRIPULANTE(tpConfig-> PUERTO_MI_RAM_HQ, id_tripulante, estado_tripulante, posX, posY, tareaVacia, punteroVacio);
	log_info(discordiador_log, "Se inicializo el Tripulante %d", id_tripulante);
	//generarSemaforoTripulantre()
}

void expulsar_tripulante(int id_tripulante) {
	Serialize_PackAndSend_TRIPULANTE_ID(tpConfig-> PUERTO_MI_RAM_HQ, id_tripulante, d_ELIMINAR_TRIPULANTE);
}

void pausar_planificaion(){
	pausar_semaforos();
}

void inciar_planificacion(){
	reanudar_semaforos();
}


void listar_tripulantes(){
	time_t t;
    t = time(NULL);
    HeaderAmongOS header;
    int cliente;
    void* packTripulante = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
    uint32_t idMensaje;
    int contador = 0;
    struct tm tm = *localtime(&t);
	printf("Estado de la Nave:	%d-%d-%d	%d:%d:%d", tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
	Serialize_PackAndSend_IMPRIMIR_TRIPULANTE(tpConfig-> PUERTO_MI_RAM_HQ, d_DAME_TRIPULANTE);

	while(id_tripulante >= contador){
		t_tripulante_mensaje *dato_temporal;
		Serialize_Unpack_TRIPULANTE(&packTripulante, &idMensaje, &dato_temporal->id, &dato_temporal->posX, &dato_temporal->posY, &dato_temporal->dir_sig_instruccion, &dato_temporal->dir_patota);
		list_add_sorted(lista_imprimir_tripulantes, dato_temporal,(void*)condicion_id_mayor_listar_tripulantes);
		contador++;
	}
	
	imprimir_lista(lista_imprimir_tripulantes);
}

bool condicion_id_mayor_listar_tripulantes(t_tripulante elemento1,t_tripulante elemento2){
	if(elemento1.id > elemento2.id){
		return true;
	}else{
		return false;
	} 
}

void imprimir_lista(t_list *lista){
	while(lista != NULL){
		t_tripulante *unTripulante = list_remove(lista ,0);

		switch(unTripulante->estado){
			case 'n':
			printf("Tipulante: %d	Patota: %d	Estado: NEW",unTripulante->id, unTripulante->id_patota);
			break;
			case 'r':
			printf("Tipulante: %d	Patota: %d	Estado: READY",unTripulante->id, unTripulante->id_patota);
			break;
			case 'e':
			printf("Tipulante: %d	Patota: %d	Estado: EXEC",unTripulante->id, unTripulante->id_patota);
			break;
			case 'b':
			printf("Tipulante: %d	Patota: %d	Estado: BLOQUEADO",unTripulante->id, unTripulante->id_patota);
			break;
			case 'f':
			printf("Tipulante: %d	Patota: %d	Estado: FINALIZADO",unTripulante->id, unTripulante->id_patota);
			break;
		}
	}
	free(lista);
}

///////////////////////////////////////////////
// ALGORITMOS PARA ENVIAR Y RECIBIR MENSAJES //
///////////////////////////////////////////////

void* recibirYAtenderFileSystem(p_elementoDeHilo* elemento) {
	while (SEGUIR_ATENDIENDO) {
		sem_wait(&semaforoFILESYSTEM);
		HeaderAmongOS headerRecibido = Serialize_RecieveHeader(elemento->cliente);
		if (headerRecibido.tipoMensaje == -1) {
			log_error(elemento->log, "Se desconecto el cliente\n");
			sem_post(&semaforoFILESYSTEM);
			break;
		}
		atender(headerRecibido, elemento->cliente, elemento->log);
		sem_post(&semaforoFILESYSTEM);
	}
	free(elemento);
	return 0;
}

void* atenderFileSystem() {
	sem_t semaforoSocket;
	sem_init(&semaforoSocket,0,1);
	fileSystem_log = iniciar_log("FILE SYSTEM");
	int conexion = iniciar_servidor(tpConfig->IP_I_MONGO_STORE, tpConfig->PUERTO_I_MONGO_STORE, fileSystem_log);
	while (SEGUIR_ATENDIENDO) {
		int cliente = esperar_cliente_con_accept(conexion, fileSystem_log);
		sem_wait(&semaforoFILESYSTEM);
		log_info(discordiador_log, "se conecto cliente: %i", cliente);
		pthread_t* dondeSeAtiende = malloc(sizeof(pthread_t));
		sem_wait(&semaforoSocket);
		p_elementoDeHilo *elemento = malloc(sizeof(p_elementoDeHilo));
		elemento->log = fileSystem_log;
		elemento->cliente = cliente;
		sem_post(&semaforoSocket);

		if (pthread_create(dondeSeAtiende, NULL,
				(void*) recibirYAtenderFileSystem, elemento) == 0) {
			log_info(fileSystem_log, ":::: Se creo hilo para cliente ::::");
		} else {
			log_error(discordiador_log,
					":::: No se pudo crear el hilo para cliente ::::");
		}
		pthread_detach(*dondeSeAtiende);
		sem_post(&semaforoFILESYSTEM);
	}
	return NULL;
}

void atenderSabotaje(t_tarea unSabotaje){
	t_list *colaSabotaje = list_create();
	HAY_SABOTAJE = true;
	t_tarea tareaAntigua;
	cargoLosDeExec(colaSabotaje,listaExec);
	cargoLosDeBlocked(colaSabotaje,listaBlocked);
	cargoLosDeReady(colaSabotaje,listaReady);
	pausar_semaforos();
	pausar_tripulantes(colaSabotaje);
	t_tripulante* tripulanteCercano = tripulanteMasCercano(unSabotaje, colaSabotaje);
	tareaAntigua = tripulanteCercano->tareaActual;
	tripulanteCercano->tareaActual = unSabotaje;

	sem_post(&tripulanteCercano->semaforoDeTripulante);
	while(HAY_SABOTAJE){
		switch(atoi(unSabotaje.nombre)){
			case cantidad_bloques:;
			funcionSolucionCantBloques();
			break;
			case bitmap:;
			funcionSolucionBitMap();
			break;
			case size:;
			funcionSolucionSize();
			break;
			case block_count:;
			funcionSolucionBlock_Count();
			break;
			case blocks:;
			funcionSolucionBlocks();
			break;
		}
	}

	tripulanteCercano->tareaActual = tareaAntigua;
	reanudar_tripulantes(colaSabotaje);
}

void atender(HeaderAmongOS header, int cliente, t_log* logger) {
	switch (header.tipoMensaje){
	case d_SABOTAJE:;
		log_info(discordiador_log, "Llego un SABOTAJE: RIP, MEGA F, SE ROMPIO TO' PAH");
		void* packTripulante = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
    	sem_post(&semaforoSocket);
		t_tarea sabotaje;
		uint32_t idMensaje, tipoSabotaje, posX, posY, duracion; //Pasar everything lo que es int a uint32_t
		Serialize_Unpack_SABOTAJE(packTripulante, &idMensaje, &tipoSabotaje, &posX, &posY, &duracion);
		atenderSabotaje(sabotaje);
		log_info(logger, "Contenidos del sabotaje: Pkm: %s, x: %i, y: %i\n", tipoSabotaje, posX, posY);
		Serialize_PackAndSend_ACK(cliente, idMensaje);
		free(&header.tamanioMensaje);
		break;

	default:
		log_error(logger, "Mensaje no entendido: %i\n", header);
		void* packBasura = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		Serialize_PackAndSend_ACK(cliente, 0);
		free(packBasura);
//		sem_post(&semaforoSocket);
		break;
	}

}

void pausar_semaforos(){
	sem_wait(&semaforoCambioDeEstado);
	sem_wait(&mutex_FIFO);
	sem_wait(&mutex_RR);
	sem_wait(&mutex_ready);
	sem_wait(&mutex_blocked);
	sem_wait(&mutex_exit);
}

void reanudar_semaforos(){
	sem_post(&semaforoCambioDeEstado);
	sem_post(&mutex_FIFO);
	sem_post(&mutex_RR);
	sem_post(&mutex_ready);
	sem_post(&mutex_blocked);
	sem_post(&mutex_exit);
}

void cargarAOtraQueue(t_list *colaFinal,t_list *colaAVaciar){
	t_tripulante *temporal = list_get(colaAVaciar,0);
	list_add(colaFinal,temporal);
}

void cargoLosDeReady(t_list *colaTodos,t_list *colaAVaciar){
	list_sort(colaAVaciar,(void*) _ordenarPorId);
	while(!list_is_empty(colaAVaciar)){
		sem_wait(&mutex_ready);
		sem_wait(&mutex_blocked_sabotaje);
		cargarAOtraQueue(colaTodos,colaAVaciar);
		sem_post(&mutex_blocked_sabotaje);
		sem_post(&mutex_ready);
	}
}

void cargoLosDeExec(t_list *colaTodos,t_list *colaAVaciar){
	list_sort(colaAVaciar,(void*) _ordenarPorId); //no sirve para colas preguntar cual sirve
	while(!list_is_empty(colaAVaciar)){
		sem_wait(&mutex_exec);
		sem_wait(&mutex_blocked_sabotaje);
		cargarAOtraQueue(colaTodos,colaAVaciar);
		sem_post(&mutex_blocked_sabotaje);
		sem_post(&mutex_exec);
	}
}

void cargoLosDeBlocked(t_list *colaTodos,t_list *colaAVaciar){
	list_sort(colaAVaciar,(void*) _ordenarPorId);
	while(!list_is_empty(colaAVaciar)){
		sem_wait(&mutex_ready);
		sem_wait(&mutex_blocked_sabotaje);
		cargarAOtraQueue(colaTodos,colaAVaciar);
		sem_post(&mutex_blocked_sabotaje);
		sem_post(&mutex_blocked);
	}
}

void pausar_tripulantes(t_list *colaBlockedSabotaje){
	
	while(!list_is_empty(colaBlockedSabotaje)){
		sem_wait(&mutex_blocked_sabotaje);
		t_tripulante *tripulante =list_get(colaBlockedSabotaje,0);
		sem_wait(&tripulante->semaforoDeTripulante);
		list_add(colaBlockedSabotaje,tripulante);
		sem_post(&mutex_blocked_sabotaje);
	}
}

void reanudar_tripulantes(t_list *colaBlockedSabotaje){
	while(!list_is_empty(colaBlockedSabotaje)){
		sem_wait(&mutex_blocked_sabotaje);
		t_tripulante *tripulante = list_get(colaBlockedSabotaje,0);
		sem_post(&tripulante->semaforoDeTripulante);
		list_add(colaBlockedSabotaje,tripulante);
		sem_post(&mutex_blocked_sabotaje);
	}
}

// todo chequear que responda bien
t_tripulante* tripulanteMasCercano(t_tarea sabotaje, t_list *cola){
	t_tripulante *tripulanteAUX;
	int distanciaMinima = 0; 
	int distanciaMaxima = 0;
	
	t_tripulante *tripulanteMenor = list_get(cola,0); //falta agregar la lista
	distanciaMinima = diferenciaEntrePuntos(tripulanteMenor->posX, tripulanteMenor->posY, sabotaje.posX, sabotaje.posY);

	while(!list_is_empty(cola)){
		tripulanteAUX = list_get(cola,0);
		distanciaMaxima = diferenciaEntrePuntos(tripulanteAUX->posX, tripulanteAUX->posY, sabotaje.posX, sabotaje.posY);
		if(distanciaMaxima < distanciaMinima){
			tripulanteMenor = tripulanteAUX;
		}
	}

	return tripulanteMenor;
}

int diferenciaEntrePuntos( int tripulanteposX, int tripulanteposY, int sabotajeposX, int sabotajeposY){
	return abs( abs(sabotajeposX - tripulanteposX) + abs(sabotajeposY - tripulanteposY) );
}


