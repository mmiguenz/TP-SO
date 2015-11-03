#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <commons/config.h>
#include "cliente.h"
#include <pthread.h>
#include "hilito.h"
#include <commons/collections/list.h>
#include <commons/log.h>
#include <semaphore.h>
#include <commons/collections/queue.h>



//--variables globales
t_log* logger;
int retardo;
int cant_hilitos;
t_queue * porcentajes_CPU; //cola de porcentajes de uso de las cpus



//-- estrucctura de los parametros de conectar funcion de los hilos de cpus
typedef struct{
	int puerto_escucha_planificador;
	char* ip_conec_plani;
	int puerto_escucha_memoria;
	char* ip_conec_memoria;
	t_log* logger;
	int retardo;
}struct1;


int main(void) {

	pthread_mutex_init(&mutex, NULL);

	//--reservamos memoria para parametros de estructura de los hilosCpu
	struct1 *paramHilo;
	paramHilo=(struct1 *)malloc(sizeof(struct1));
	paramHilo->ip_conec_memoria=malloc(12);
	paramHilo->ip_conec_plani=malloc(12);

	//--creamos la cola y asignamos a la variable de la estructura
	porcentajes_CPU=queue_create();

	//--creamos arch de configuracion y sacamos la informacion (ip, puerto , retardo, cantHIlos)
	t_config* config;
	config = config_create("config.cfg");

	paramHilo->logger = log_create("log.txt", "CPU",false, LOG_LEVEL_INFO);

	if(config != NULL){
		paramHilo->puerto_escucha_planificador=config_get_int_value(config, "PORT_PLANIFICADOR");
		paramHilo->ip_conec_plani=config_get_string_value(config,"IP_PLANIFICADOR");
		paramHilo->puerto_escucha_memoria=config_get_int_value(config, "PORT_MEMORIA");
		paramHilo->ip_conec_memoria=config_get_string_value(config,"IP_MEMORIA");
		paramHilo->retardo = config_get_int_value(config, "RETARDO");
		cant_hilitos=config_get_int_value(config, "CANTIDAD_HILOS");

	}


	//--inicializamos un vector de hilosCpus con un max y un hilo para los porcentajes
	pthread_t threads[100];
	//pthread_t threadPorcentajes;

	int err;
	long t;

	//--hilo para los porcentajes
	//pthread_create(&threadPorcentajes, NULL,(void*)porcentajesCPU,&porcentajes_CPU);

	//--crea n cantidad de hilos segun el arch de configuracion, el pasamos lo param antes inicializados
	for(t=0; t<cant_hilitos; t++){
		err = pthread_create(&threads[t], NULL, (void*)conectar, (void*) paramHilo);
		if (err){
			printf("ERROR; return code from pthread_create() is %d\n", err);
			exit(-1);
		}
	}



	for(t=0; t<cant_hilitos; t++){
		pthread_join(threads[t], NULL);
	}
	//pthread_join(threadPorcentajes, NULL);
	//--liberamos memoria
	queue_destroy(porcentajes_CPU);
	pthread_mutex_destroy(&mutex);
	pthread_exit(NULL);
	config_destroy(config);
	free(paramHilo->ip_conec_memoria);
	free(paramHilo->ip_conec_plani);
	return EXIT_SUCCESS;
}
