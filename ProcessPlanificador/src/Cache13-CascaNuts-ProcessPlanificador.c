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


t_queue * fifo_PCB;



typedef struct {
char* nombreProc;
int estado;
int PID;
int contadorProgram;
char* path;
}PCB ;

//#include "PCB.h"


PCB *pcb_create(char *name, int estado, char* ruta){
	PCB *new = malloc( sizeof(PCB) );
	new->nombreProc = name;
	new->PID = 0;
	new->estado=0;
	new->contadorProgram=0;
	new->path=malloc(strlen(ruta)+1);
	new->path=ruta;
	return new;
}


#include "servidor.h"



void shell();

int tamaniobuf(char cad[]);

int esComando(char * comando);



int main(void)
{
	//Espacio para la configuracion del entorno---------------------------<<

	fifo_PCB=queue_create();
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

                        	conectar(puerto_escucha_planif, fifo_PCB, logger);


                        	pthread_join(hilo_shell, NULL);

    return 0;
}


void shell(){
	char* comando = malloc(sizeof(char*));
	char* ruta =  string_new();;
	PCB* nuevoPCB=malloc(sizeof(PCB));
	char** substring =malloc(sizeof(char**));

	printf("\n\n-----------------Bienvenido al Planificador Cache 13 V1.0----------------\n");
	printf("----Por esta consola debera ingresar los procesos que necesite correr----\n");
	printf("--------------------------------------------------------------------------\n\n\n\n");

    while(1){
    printf("Por favor ingrese el mProc que desea correr:  \n");
    fgets(comando,50,stdin);
    substring= string_split(comando, " ");
    if(!strcmp(substring[0], "correr")){
    printf("\n El mProc que eligio es %s \n",substring[1]);
    substring=string_split(substring[1],"\n");
    ruta= (char*)malloc(1+strlen("/home/utnso/workspace/tp-2015-2c-cascanueces/Procesos/") + strlen(substring[0]) + strlen(".cod"));
    strcpy(ruta, "/home/utnso/workspace/tp-2015-2c-cascanueces/Procesos/");
    strcat(ruta, substring[0]);
    strcat(ruta, ".cod");

    printf("Y su ruta de acceso es: %s \n", ruta);
    nuevoPCB = pcb_create(substring[0],0,ruta);//Creo mi pcb

    queue_push(fifo_PCB,nuevoPCB);//Voy metiendo los pcb en la cola fifo de pcb
   // sem_post(&haveData);
    }
    else{printf("el comando ingresado es incorrecto \n");}


    }


}








int tamaniobuf(char cad[])
{
   int pos = -1;
   int len = strlen( cad);
int i;
   for( i = 0; pos == -1 && i < len; i++){ // si quitas la condición pos == -1
            // te devuelve la última posición encontrada (si es que hay más de 1)
      if(*(cad+i) == '\0')
         pos = i+1;
   }
   return pos;
}

int esComando(char * comando){

if(strcmp(comando,"Correr Programa")){
return 1;
}
return 0;
}


/*
static void log_in_disk(char* temp_file) {
    t_log* logger = log_create("log.txt", "PLANIFICADOR",true, LOG_LEVEL_INFO);

    log_trace(logger, "LOG A NIVEL %s", "TRACE");
    log_debug(logger, "LOG A NIVEL %s", "DEBUG");
    log_info(logger, "LOG A NIVEL %s", "INFO");
    log_warning(logger, "LOG A NIVEL %s", "WARNING");
    log_error(logger, "LOG A NIVEL %s", "ERROR");

 ;
}
*/
