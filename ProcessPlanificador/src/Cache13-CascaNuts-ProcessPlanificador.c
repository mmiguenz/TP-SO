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
#include <pthread.h>
#include "PCB.h"
#include "servidor.h"

 t_queue * fifo_PCB;

void shell();

int tamaniobuf(char cad[]);

int esComando(char * comando);

static void log_in_disk(char* temp_file);



int main(void)
{
	//Espacio para la configuracion del entorno---------------------------<<

	t_log* logger = log_create("log.txt", "PLANIFICADOR",false, LOG_LEVEL_INFO);

	pthread_t hilo_shell; //Hilo que creo para correr el shell que acepta procesos por terminal

	char* puerto_escucha_planif;
                        	t_config* config;

                        	puerto_escucha_planif=malloc(sizeof puerto_escucha_planif);
                        	config = config_create("config.cfg");
                        	if(config != NULL){
                        	puerto_escucha_planif=config_get_string_value(config, "PORT");
                        	 log_info(logger, "Se abrio el archivo de configuracion %s", "CONFIG");
                        	}



	//----------Soy una barra separadora ;)--------------------------------------//
                        	pthread_create(&hilo_shell, NULL, shell, NULL);

                        	pthread_join(hilo_shell, NULL);

                        	conectar(puerto_escucha_planif);


    return 0;
}


void shell(){
	char* comando = malloc(sizeof(char*));
	printf("-----------------Bienvenido al Planificador Cache 13 V1.0----------------\n");
	printf("----Por esta consola debera ingresar los procesos que necesite correr----\n");
	printf("--------------------------------------------------------------------------\n\n\n\n");

    while(1){
    printf("Por favor ingrese el mProc que desea correr:  \n");
    fgets(comando,50,stdin);
    printf("\n El mProc que eligio es %s \n",comando);


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



static void log_in_disk(char* temp_file) {
    t_log* logger = log_create("log.txt", "PLANIFICADOR",true, LOG_LEVEL_INFO);

    log_trace(logger, "LOG A NIVEL %s", "TRACE");
    log_debug(logger, "LOG A NIVEL %s", "DEBUG");
    log_info(logger, "LOG A NIVEL %s", "INFO");
    log_warning(logger, "LOG A NIVEL %s", "WARNING");
    log_error(logger, "LOG A NIVEL %s", "ERROR");

 ;
}
