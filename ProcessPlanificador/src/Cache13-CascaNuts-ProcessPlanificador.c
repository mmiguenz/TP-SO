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
#include <servidor.h>
#include <PCB.h>



void shell(int listener, int skEmisor, int skReceptor, char * buf, int nbytes);

int tamaniobuf(char cad[]);

int esComando(char * comando);

int main(void)
{
	//Espacio para la configuracion del entorno---------------------------<<
 char* puerto_escucha_planif;
                        	t_config* config;

                        	puerto_escucha_planif=malloc(sizeof puerto_escucha_planif);
                        	config = config_create("config.cfg");
                        	if(config != NULL){
                        	puerto_escucha_planif=config_get_string_value(config, "PORT");}


	//----------Soy una barra separadora ;)--------------------------------------//
                        	conectar_servidor(puerto_escucha_planif);

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



