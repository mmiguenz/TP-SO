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

t_log* logger;
int retardo;
int cant_hilitos;

t_queue * porcentajes_CPU;

typedef struct{
	int puerto_escucha_planificador;
	char* ip_conec_plani;
	int puerto_escucha_memoria;
	char* ip_conec_memoria;
	t_log* logger;
	int retardo;
	t_queue * porcentajes_CPU;
}struct1;


int main(void) {

	struct1 *paramHilo;
	paramHilo=(struct1 *)malloc(sizeof(struct1));
	porcentajes_CPU=queue_create();
/*
	int puerto_escucha_planificador;
	int puerto_escucha_memoria;
	char* ip_conec_plani;
	char* ip_conec_memoria;

	ip_conec_plani= malloc(sizeof (ip_conec_plani));
	ip_conec_memoria= malloc(sizeof (ip_conec_memoria));
*/
	t_config* config;
	config = config_create("config.cfg");
	paramHilo->logger = log_create("log.txt", "CPU",false, LOG_LEVEL_INFO);
	paramHilo->ip_conec_memoria=malloc(12);
	paramHilo->ip_conec_plani=malloc(12);
	paramHilo->porcentajes_CPU= porcentajes_CPU;

	if(config != NULL){
		paramHilo->puerto_escucha_planificador=config_get_int_value(config, "PORT_PLANIFICADOR");
		paramHilo->ip_conec_plani=config_get_string_value(config,"IP_PLANIFICADOR");
		paramHilo->puerto_escucha_memoria=config_get_int_value(config, "PORT_MEMORIA");
		paramHilo->ip_conec_memoria=config_get_string_value(config,"IP_MEMORIA");
		paramHilo->retardo = config_get_int_value(config, "RETARDO");
		cant_hilitos=config_get_int_value(config, "CANTIDAD_HILOS");

	}

	printf("Cantidad de hilitos es: %d\n", cant_hilitos);
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	/*while(cant_hilitos){

			pthread_t hilito;
			struct param param1 = { puerto_escucha_planificador,ip_conec_plani, puerto_escucha_memoria, ip_conec_memoria, logger,retardo};
			pthread_create(&hilito, NULL, (void*)conectar,(void*)&param1 );
			pthread_join(hilito, NULL);
(pthread_mutex_init(&lock, NULL)


		}*/

	pthread_t threads[100];
	int err;
	long t;
	pthread_t threadPorcentajes;

	pthread_create(&threadPorcentajes, NULL,(void*)porcentajesCPU,&porcentajes_CPU);


	for(t=0; t<cant_hilitos; t++){


		//struct param param1 = { puerto_escucha_planificador,ip_conec_plani, puerto_escucha_memoria, ip_conec_memoria, logger,retardo,};

		err = pthread_create(&threads[t], NULL, (void*)conectar, (void*) paramHilo);

		if (err){
			printf("ERROR; return code from pthread_create() is %d\n", err);
			exit(-1);
		}

	}
	pthread_join(threadPorcentajes, NULL);

	for(t=0; t<cant_hilitos; t++){
		pthread_join(threads[t], NULL);
	}

	//free(a);
	pthread_exit(NULL);
	config_destroy(config);
	//int  pthread_mutex_destroy(pthread_mutex_t * mutex);
	//free ( ip_conec_memoria);
	//free ( ip_conec_plani);
	return EXIT_SUCCESS;
}
