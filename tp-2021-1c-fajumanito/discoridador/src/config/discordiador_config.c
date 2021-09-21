
//Cargo las configuraciones del .config
	config = leerConfiguracion("/home/utnso/workspace/tp-2020-2c-Los-Recursa2/configs/app.config");
	if(config != NULL)
	{
		puts("Archivo de configuración cargado correctamente.");
	}
	else
	{
		puts("Error al cargar archivo de configuración, abortando...\n");
		abort();
	}