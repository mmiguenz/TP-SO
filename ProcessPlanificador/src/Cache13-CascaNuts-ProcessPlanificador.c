/* ProcessPlanificador
 *
 *  Created on: 6/9/2015
 *      Author:Sebastian A. Agosta
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#include <regex.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

/*-------Comandos del Shell-------*/
#define CORRER 0
#define FINALIZAR 1
#define PS 2
#define CPU 3
#define ERROR -1

//sem_t haveData;


t_queue * fifo_PCB_ready;//Cola de pcb que estan listo para ejecutar
t_queue * PCB_running;//Cola de pcb que estan ejecutando
t_queue * block_PCB; //Cola de pcb que estan blockeados

sem_t sem_mutex1;
sem_t sem_consumidor;
sem_t sem_mutex_block;
sem_t sem_consumidor_block;



typedef struct {
char* nombreProc;
int estado;
int PID;
int contadorProgram;
char* path;
int cpu_asignada;
int quantum;//Si el quantum es -1 la planificacion es fifo caso contrario round robin
int retardo_io;

}PCB ;

//#include "PCB.h"


int generar_pid();

void recolectar_comando(char comando[15]);

void  recolectar_proceso(char proceso[15]);

int  recolectar_pid(char proceso[]);

void procesar_comando(char comando[15], char proceso[15],int mutex, int cpu_conectada);

PCB *pcb_create(char *name, int estado, char* ruta);

int identificar_comando(char comando[]);

#include "servidor.h"
#include "semaph.h"



void* shell(int mutex);

void * manejo_IO();
int quantum=0;
key_t claveMutex;

int main(void)
{
	//Espacio para la configuracion del entorno---------------------------<<

	fifo_PCB_ready=queue_create();
	PCB_running=queue_create();
	block_PCB=queue_create();
	char* planificacion;
	t_log* logger= log_create("log.txt", "PLANIFICADOR",false, LOG_LEVEL_TRACE);

	pthread_t hilo_shell; //Hilo que creo para correr el shell que acepta procesos por terminal
	pthread_t hilo_io; //Hilo que creo para correr la i/o  que acepta procesos y los blockea


	char* puerto_escucha_planif;

                        	t_config* config;

                        	puerto_escucha_planif=malloc(sizeof puerto_escucha_planif);
                        	config = config_create("config.cfg");
                        	if(config != NULL){
                        	puerto_escucha_planif=config_get_string_value(config, "PORT");
                        	planificacion=config_get_string_value(config,"PLANIFICACION");
                        	if(!strcmp(planificacion,"ROUNDROBIN")){quantum=config_get_int_value(config,"QUANTUM");};

                        	 log_info(logger, "Se abrio el archivo de configuracion %s", "CONFIG");
                        	}
//------------------Soy una barra separadora ;p------------------------------------//
//------------------Creacion de semaforos------------------------------------------//

   int   mutex;	/* semáforo */

   /* obtener una clave cualquiera para el recurso ipc */
   if ((key_t) -1 == (claveMutex = ftok("/bin/ls", 123))) {
	   fprintf(stderr, "main: Error al crear la clave con ftok()\n");
       exit(1);
       }
    /* crear del semaforo */
     if (-1 == (mutex = semCreate(claveMutex, 1))){
     fprintf(stderr, "main: No pude crear el semaforo\n");
     exit(1);
       }


/********************Soy una barra llena de asteriscos*********************************************/

				pthread_create(&hilo_shell, NULL, shell, (void *) &mutex);

				pthread_create(&hilo_io, NULL, manejo_IO, NULL);



				conectar_fifo(puerto_escucha_planif, fifo_PCB_ready, logger, PCB_running,
						mutex, block_PCB);

				pthread_join(hilo_shell, NULL);
				pthread_join(&hilo_io, NULL);
    return 0;
}



/*Shell funcion que se carga en el hilo y va aceptando procesos----->>*/

void *shell(int mutex){
	char comando[15]; //= malloc(sizeof(char*));
	char proceso[15];
	//sem_open("sem_consumidor",O_CREAT,0644,0);


	printf("\n\n-----------------Bienvenido al Planificador Cache 13 V3.5----------------\n");
	printf("----Por esta consola debera ingresar los procesos que necesite correr----\n");
	printf("----o bien los comandos que desea que realize el planificador------------\n");
	printf("--------------------------------------------------------------------------\n\n\n\n");
	  if (-1 == (mutex = semOpen(claveMutex)))
		 fprintf(stderr, "No tengo el cualificador de mutex\n");

	    sem_init(&sem_mutex1, 1, 1);
	    sem_init(&sem_consumidor,1,0);
	    sem_init(&sem_mutex_block,1,0);

	/************Barra separadora que crea una conexion con hilo porcentaje CPU******///////
	    int fd, Cpu_conectada; /* los ficheros descriptores */

	       struct sockaddr_in server;
	       /* para la información de la dirección del servidor */

	       struct sockaddr_in client;
	       /* para la información de la dirección del cliente */

	       int sin_size;

	       /* A continuación la llamada a socket() */
	       if ((fd=socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
	          printf("error en socket()\n");
	          exit(-1);
	       }

	       server.sin_family = AF_INET;

	       server.sin_port = htons(8082);

	       server.sin_addr.s_addr = INADDR_ANY;

	       bzero(&(server.sin_zero),8);

	        if(bind(fd,(struct sockaddr*)&server,
	                sizeof(struct sockaddr))==-1) {
	           printf("error en bind() \n");
	           exit(-1);
	        }

	        if(listen(fd,1) == -1) {  /* llamada a listen() */
	           printf("error en listen()\n");
	           exit(-1);
	        }

	        sin_size=sizeof(struct sockaddr_in);
	            /* A continuación la llamada a accept() */
	            if ((Cpu_conectada = accept(fd,(struct sockaddr *)&client,
	                              &sin_size))==-1) {
	               printf("error en accept()\n");
	               exit(-1);
	            }

	       while(1){
    printf("Por favor ingrese el comando que desea ejecutar:  \n");
    sem_init(&sem_mutex1, 1, 1);
    sem_init(&sem_mutex_block,1,1);

    recolectar_comando(comando);

    procesar_comando(comando, proceso, mutex,Cpu_conectada);

    }


}

 void * manejo_IO()
 {PCB* pcb_block;


	 while(1){
		pcb_block = malloc(sizeof(PCB*));
		pcb_block->nombreProc = malloc(50);
		pcb_block->path = malloc(200);
		sem_wait(&sem_consumidor_block);
		pcb_block = queue_pop(block_PCB);

		sleep(pcb_block->retardo_io);

		queue_push(fifo_PCB_ready,pcb_block);

	 }
 }



/*
 * Funcion que  toma por pantalla el comando caracter por caracter
 *formando mi comando
 *tomo como premisa que los mismos no superaran los 15 caracteres.
 */
void recolectar_comando(char comando[])
{   /*-----------------Leo el Comando---------------------------------*/
    int i =0;
    scanf("%c",&comando[i]);
    while ((comando[i]!=' ') & (comando[i]!='\n')){
    	i++;
    scanf("%c",&comando[i]);
    }
    comando[i]='\0';


 }

/*
 * Funcion que  toma por pantalla el proceso caracter por caracter
 *formando mi comando
 *tomo como premisa que los mismos no superaran los 15 caracteres.
 */
void  recolectar_proceso(char proceso[]){
	   /*---------------Leo El proceso ---------------------------------*/
	    int i =0;
	        scanf("%c",&proceso[i]);
	        while (proceso[i]!='\n'){
	        	i++;
	        scanf("%c",&proceso[i]);
	        }
	        proceso[i]='\0';

}


int  recolectar_pid(char proceso[]){
	   /*---------------Leo El pid ---------------------------------*/

	int i =0;
	        scanf("%c",&proceso[i]);
	        while (proceso[i]!='\n'){
	        	i++;
	        scanf("%c",&proceso[i]);
	        }
	        proceso[i]='\0';
	        return atoi(proceso);
}


/*
 * Funcion que  toma como parametros el comando y el proceso
 * verifica que comando es y actua de acuerdo al mismo con el proceso
 * seleccionado
 */

void procesar_comando(char comando[], char proceso[],int mutex, int cpu_conectada){

	char* ruta =  string_new();

	PCB* nuevoPCB=malloc(sizeof(PCB));


	switch(identificar_comando(comando)){
	case CORRER:{
		recolectar_proceso(proceso);



    /*----------------Genero la ruta del proceso--------------------*/
    ruta= malloc(1+strlen("/home/utnso/workspace/tp-2015-2c-cascanueces/Procesos/") + strlen(proceso) + strlen(".cod"));
    strcpy(ruta, "/home/utnso/workspace/tp-2015-2c-cascanueces/Procesos/");
    strcat(ruta, proceso);
    strcat(ruta, ".cod");

    printf("Y su ruta de acceso es: %s \n", ruta);

    /*-----------------Creo mi PCB----------------------------------*/

    nuevoPCB = pcb_create(proceso,0,ruta);//Creo mi pcb

    //sem_wait(&sem_mutex1);
    queue_push(fifo_PCB_ready,nuevoPCB);//Voy metiendo los pcb en la cola fifo de pcb
    sem_post(&sem_consumidor);
    //sem_post(&sem_mutex1);
    break;
    }
	case FINALIZAR:
	{
		int pid;
		PCB* auxPCB=malloc(sizeof(PCB));


		pid=recolectar_pid(proceso);
		printf("El Comando que eligio fue finalizar \n");
		printf("Y el PID del proceso a finalizar es = %d  \n",pid);
		int tamanio=queue_size(fifo_PCB_ready);
		semWait(mutex);
		while(tamanio!=0){
		auxPCB=queue_pop(fifo_PCB_ready);
		printf("El pid del proceso es: %d \n",auxPCB->PID);
		if(auxPCB->PID==pid)
		{printf("Encontre  al proceso!!!\n");
		auxPCB->contadorProgram=-1;}//Si el cpu encuentra -1 lo finaliza automaticamente

		queue_push(fifo_PCB_ready,auxPCB);
		tamanio--;
		}
		semSignal(mutex);
		break;
	}
	case PS:
	{
		printf("El comando que eligio fue ps \n");
		PCB* auxPCBrun=malloc(sizeof(PCB));
		PCB* auxPCBblock=malloc(sizeof(PCB));
		int tamanio=queue_size(fifo_PCB_ready);
		semWait(mutex);
		while(tamanio!=0){
		auxPCBrun=queue_pop(fifo_PCB_ready);
		printf("mProc %d: %s -> Listo \n",auxPCBrun->PID,auxPCBrun->nombreProc);
		queue_push(fifo_PCB_ready,auxPCBrun);
		tamanio--;
		}
		tamanio=queue_size(PCB_running);
		while(tamanio!=0){
		auxPCBrun=queue_pop(PCB_running);
		printf("mProc %d: %s -> Corriendo \n",auxPCBrun->PID,auxPCBrun->nombreProc);
		queue_push(PCB_running,auxPCBrun);
		tamanio--;
		}
		tamanio=queue_size(block_PCB);
		while(tamanio!=0){
		auxPCBrun=queue_pop(block_PCB);
		printf("mProc %d: %s -> Blockeado \n",auxPCBblock->PID,auxPCBblock->nombreProc);
		queue_push(block_PCB,auxPCBblock);
		tamanio--;
		}
		semSignal(mutex);

		break;
	}
	case CPU:
	{
		printf("El comando que eligio fue cpu \n");
		send(cpu_conectada,"Dame %", 8, 0);
		break;
	}
	case ERROR:
	{
		printf("No reconozco ese comando tipee de nuevo \n");
	}
	}

}

int identificar_comando(char comando[]){
	int com =-1;

	if(!strcmp(comando, "correr")){com=0;}
	if(!strcmp(comando, "finalizar")){com=1;}
	if(!strcmp(comando, "ps")){com=2;}
	if(!strcmp(comando, "cpu")){com=3;}

	return com;


}



/*
 * Funcion que genera un pcb de proceso dandole un nombre una ruta y un estado
 * devolviendo una estructura de tipo PCB
 */

PCB *pcb_create(char *name, int estado, char* ruta){
	PCB *new = malloc( sizeof(PCB) );
	new->nombreProc=malloc(strlen(name)+1);
	new->path=malloc(strlen(ruta)+1);
	strcpy(new->nombreProc, name);
	new->PID = generar_pid();
	new->estado=0;
	new->contadorProgram=0;
	new->path=ruta;
	new->quantum=quantum;

	return new;
}


int generar_pid(){return rand()/10000000+rand()%100;}
