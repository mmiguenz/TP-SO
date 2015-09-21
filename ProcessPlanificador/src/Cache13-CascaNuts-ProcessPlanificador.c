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
//#include "servidor.c"

 t_queue * fifo_PCB;

void shell(int listener, int skEmisor, int skReceptor, char * buf, int nbytes);

int tamaniobuf(char cad[]);

int esComando(char * comando);

static void log_in_disk(char* temp_file);



int main(void)
{
	//Espacio para la configuracion del entorno---------------------------<<
    t_log* logger = log_create("log.txt", "PLANIFICADOR",false, LOG_LEVEL_INFO);
	char* puerto_escucha_planif;
                        	t_config* config;

                        	puerto_escucha_planif=malloc(sizeof puerto_escucha_planif);
                        	config = config_create("config.cfg");
                        	if(config != NULL){
                        	puerto_escucha_planif=config_get_string_value(config, "PORT");
                        	 log_info(logger, "Se abrio el archivo de configuracion %s", "CONFIG");
                        	}



	//----------Soy una barra separadora ;)--------------------------------------//
                        	conectar(puerto_escucha_planif);

    return 0;
}


void shell(int listener, int skEmisor, int skReceptor, char * buf, int nbytes){


    printf("%s\n", buf);//action);


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
