/* ProcessPlanificador
 *
 *  Created on: 6/9/2015
 *      Author: Cascanuts
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

//sem_t haveData;


t_queue * fifo_PCB_ready;//Cola de pcb que estan listo para ejecutar

sem_t sem_productor;
sem_t sem_consumidor;

typedef struct {
char* nombreProc;
int estado;
int PID;
int contadorProgram;
char* path;
int cpu_asignada;

}PCB ;

//#include "PCB.h"


PCB *pcb_create(char *name, int estado, char* ruta){
	PCB *new = malloc( sizeof(PCB) );
	new->nombreProc=malloc(sizeof(char*));
	new->nombreProc = name;
	new->PID = rand();
	new->estado=0;
	new->contadorProgram=0;
	new->path=malloc(sizeof(char*));
	new->path=ruta;

	return new;
}


#include "servidor.h"



void* shell();



int main(void)
{
	//Espacio para la configuracion del entorno---------------------------<<

	fifo_PCB_ready=queue_create();
	t_log* logger= log_create("log.txt", "PLANIFICADOR",false, LOG_LEVEL_INFO);

	pthread_t hilo_shell; //Hilo que creo para correr el shell que acepta procesos por terminal

	char* puerto_escucha_planif;
                        	t_config* config;

                        	puerto_escucha_planif=malloc(sizeof puerto_escucha_planif);
                        	config = config_create("config.cfg");
                        	if(config != NULL){
                        	puerto_escucha_planif=config_get_string_value(config, "PORT");
                        	 log_info(logger, "Se abrio el archivo de configuracion %s", "CONFIG");
                        	}
//------------------Soy una barra separadora ;p------------------------------------//

                        	//sem_init(&haveData, 0, 0);


/********************Soy una barra llena de asteriscos*********************************************/


                        	pthread_create(&hilo_shell, NULL, shell, NULL);

                        	conectar_fifo(puerto_escucha_planif, fifo_PCB_ready, logger);


                        	pthread_join(hilo_shell, NULL);

    return 0;
}



/*Shell funcion que se carga en el hilo y va aceptando procesos----->>*/

void *shell(){
	char comando[15]; //= malloc(sizeof(char*));
	char proceso[15];

	char* ruta =  string_new();;
	PCB* nuevoPCB=malloc(sizeof(PCB));

	printf("\n\n-----------------Bienvenido al Planificador Cache 13 V1.0----------------\n");
	printf("----Por esta consola debera ingresar los procesos que necesite correr----\n");
	printf("--------------------------------------------------------------------------\n\n\n\n");

    while(1){
    printf("Por favor ingrese el mProc que desea correr:  \n");

    /*-----------------Leo el Comando---------------------------------*/
    int i =0;
    scanf("%c",&comando[i]);
    while (comando[i]!=' '){
    	i++;
    scanf("%c",&comando[i]);
    }
    comando[i]='\0';
    /*---------------Leo El proceso ---------------------------------*/
    i =0;
        scanf("%c",&proceso[i]);
        while (proceso[i]!='\n'){
        	i++;
        scanf("%c",&proceso[i]);
        }
        proceso[i]='\0';

    /*------------------Verifico el comando que sea correcto--------*/
    if(!strcmp(comando, "correr")){
    printf("\n El mProc que eligio es %s \n",proceso);


    /*----------------Genero la ruta del proceso--------------------*/
    ruta= malloc(1+strlen("/home/utnso/workspace/tp-2015-2c-cascanueces/Procesos/") + strlen(proceso) + strlen(".cod"));
    strcpy(ruta, "/home/utnso/workspace/tp-2015-2c-cascanueces/Procesos/");
    strcat(ruta, proceso);
    strcat(ruta, ".cod");

    printf("Y su ruta de acceso es: %s \n", ruta);

    /*-----------------Creo mi PCB----------------------------------*/

    nuevoPCB = pcb_create(proceso,0,ruta);//Creo mi pcb

    queue_push(fifo_PCB_ready,nuevoPCB);//Voy metiendo los pcb en la cola fifo de pcb
    sem_post(&sem_consumidor);
    }
    else{printf("el comando ingresado es incorrecto \n");}


    }


}



