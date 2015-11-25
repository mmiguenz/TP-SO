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

void * manejo_porcentajes(void* mensa);

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
	struct1 *param_porc;
	paramHilo=(struct1 *)malloc(sizeof(struct1));
	paramHilo->ip_conec_memoria=malloc(12);
	paramHilo->ip_conec_plani=malloc(12);
	param_porc=(struct1 *)malloc(sizeof(struct1));
	param_porc->ip_conec_memoria=malloc(12);
	param_porc->ip_conec_plani=malloc(12);




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

		param_porc->puerto_escucha_planificador=config_get_int_value(config, "PORT_PLANIFICADOR");
		param_porc->ip_conec_plani=config_get_string_value(config,"IP_PLANIFICADOR");
		param_porc->puerto_escucha_memoria=config_get_int_value(config, "PORT_MEMORIA");
		param_porc->ip_conec_memoria=config_get_string_value(config,"IP_MEMORIA");
		param_porc->retardo = config_get_int_value(config, "RETARDO");

	}


	//--inicializamos un vector de hilosCpus con un max y un hilo para los porcentajes
	pthread_t threads[100];
	//pthread_t threadPorcentajes;

	int err;
	long t;


 	pthread_t hilo_porcentajes;
 	pthread_t manejo_porcentajes;

 	pthread_create(&hilo_porcentajes, NULL, (void*)calcularPorcentajes, NULL);
 	pthread_create(&manejo_porcentajes, NULL,(void*)mensajear_porc, (void*) paramHilo);



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
	pthread_join(hilo_porcentajes, NULL);
	pthread_join(manejo_porcentajes, NULL);
	queue_destroy(porcentajes_CPU);
	pthread_mutex_destroy(&mutex);
	pthread_exit(NULL);
	config_destroy(config);
	free(paramHilo->ip_conec_memoria);
	free(paramHilo->ip_conec_plani);
	return EXIT_SUCCESS;
}




void * manejo_porcentajes(void* mensa){

	//--inicializamos estrucctura para recibir parametros
	struct1 *param;
	//param=malloc(sizeof(struct1));
	param=(struct1*)mensa;

	//las asignamos a variables locales
	int puertoPlanificador = param->puerto_escucha_planificador;
	char* ipPlanificador = param ->ip_conec_plani;
	int puertoMemoria = param-> puerto_escucha_memoria;
	char* ipMemoria = param->ip_conec_memoria;
	t_log* logger = param->logger;
	retardo = param->retardo;
	int cpu;


	//--nos conectamos con planificador y memoria
	int planificador = conectar_cliente(puertoPlanificador, ipPlanificador);
	int memoria = conectar_cliente(puertoMemoria, ipMemoria);
	inicializarInstrucciones();
	//--verificamos que nos hayamos conectado correctamente a memoria, loggueamos
	if (memoria > 0) {
		//log_info(logger, "El hilo %u se conecto a memoria correctamente", (unsigned int) pthread_self());
	} else {
		log_info(logger, "error al conectarse con memoria");
	}




	char* aux = recibirMensaje(planificador);

	t_msgHeader header2;
			memset(&header2, 0, sizeof(t_msgHeader));
			header2.msgtype = 7; //significa kien soy
			header2.payload_size = planificador;
			send(planificador, &header2, sizeof( t_msgHeader), 0);

			recv(planificador,&cpu,sizeof(int),0);
			printf("Soy el CPU %d \n\n",cpu);




	while(1){
		//avisamos al planificador que estamos listo para recibir un mProc
		//primera estructura
		t_msgHeader header2;
		memset(&header2, 0, sizeof(t_msgHeader));
		header2.msgtype = 0; //significa estoy libre
		header2.payload_size = planificador;
		send(planificador, &header2, sizeof( t_msgHeader), 0);

		//inicializamos variables para calcular el porcentaje de uso
		time_t comienzo, final;
		comienzo = time( NULL );


		//primero recibimos el tamaño del msj del planificador
		t_msgHeader header;
		memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
		recv(planificador, &header, sizeof( t_msgHeader), 0);

		//recibimos el msj del planificador pcb
		char* buffer;
		PCB *PcbAux =malloc(sizeof(PCB));
		PcbAux->PID=0;
		PcbAux->contadorProgram=0;
		PcbAux->cpu_asignada=0;
		PcbAux->nombreProc=malloc(50);
		PcbAux->path=malloc(200);
		PcbAux->quantum=0;

		buffer=malloc(header.payload_size+5);
		recv(planificador, buffer, header.payload_size, 0);

		int offset=0;
		memcpy(&PcbAux->PID,buffer +offset  ,  sizeof(int));
		offset+=sizeof(int);
		memcpy(&PcbAux->contadorProgram,buffer +offset, sizeof(int));
		offset+=sizeof(int);
		memcpy(&PcbAux->cpu_asignada,buffer +offset  ,  sizeof(int));
		offset+=sizeof(int);
		PcbAux->path=strdup(buffer+offset);
		offset+=strlen(PcbAux->path)+1;
		PcbAux->nombreProc=strdup(buffer +offset);
		offset+=strlen(PcbAux->nombreProc)+1;
		memcpy(&PcbAux->quantum,buffer +offset  ,  sizeof(int));


		printf("El mensaje del planificador PCB es:\n");
		printf("Este es el PID:                  %d\n", PcbAux->PID);
		printf("Este es el contador de programa: %d\n",PcbAux->contadorProgram);
		printf("La CPU asignada es:              %d\n",PcbAux->cpu_asignada);
		printf("El nombre  del proceso es:       %s\n", PcbAux->nombreProc);
		printf("El path es:                      %s\n", PcbAux->path);
		printf("El quantum es:                   %d\n", PcbAux->quantum);
/*
		log_info(logger, "----------------PCB-------------------------------------------------------------------------------------------------------------------------------------------------------------");
		log_info(logger, "Este es el PID:                  %d\n", PcbAux->PID);
		log_info(logger, "Este es el contador de programa: %d\n",PcbAux->contadorProgram);
		log_info(logger, "La CPU asignada es:              %d\n",PcbAux->cpu_asignada);
		log_info(logger, "El nombre  del proceso es:       %s\n", PcbAux->nombreProc);
		log_info(logger, "El path es:                      %s\n", PcbAux->path);
		log_info(logger, "El quantum es:                   %d\n", PcbAux->quantum);
		log_info(logger, "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
*/

		//contadorProgram; si es -1 no abrimos, ejecutamos solo FINALIZAR
		if(PcbAux->contadorProgram!= -1){
			//abrimos el mProc y lo guardamos en un buffet para leerlo
			char archivoProc[1500];
			memset(archivoProc,'\0', 1500);
			abrir(PcbAux,archivoProc);

			// si el quantum es 0 la planificacion es FIFO, sino es ROUND ROBIN
			if(PcbAux->quantum == 0){
				//fifo
				printf("El quantum es %d\n", PcbAux->quantum);
				procesarCadena(archivoProc, memoria, planificador,logger, PcbAux, retardo, cpu);

			}else{
				printf("El quantum es %d\n", PcbAux->quantum);
				procesarCadenaConQuantum(PcbAux->quantum, archivoProc, memoria, planificador, logger, PcbAux, retardo,cpu);
			}
		}else{
			sentenciaFinalizar(memoria, planificador, logger, PcbAux, retardo);
		}

		free(buffer);
	}


	return EXIT_SUCCESS;
}
