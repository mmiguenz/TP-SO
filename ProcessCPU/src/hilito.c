/*
 * hilito.c
 *
 *  Created on: 19/9/2015
 *      Author: utnso
 */


/*
 ============================================================================
 Name        : hilito.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#ifndef HILITO_C_
#define HILITO_C_
#include <stdio.h>
#include <stdlib.h>
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
#include <commons/log.h>
#include <pthread.h>
#include <commons/string.h>

#include <cliente.h>
#include <assert.h>
#include "hilito.h"

void conectar(){
	int puerto_escucha_planificador;
	char* ip_conec_plani;
	ip_conec_plani= malloc(sizeof ip_conec_plani);
	 t_config* config;

	 int puerto_escucha_memoria;
	 char* ip_conec_memoria;
	 ip_conec_memoria= malloc(sizeof ip_conec_memoria);


	                        	config = config_create("config.cfg");
	                        	if(config != NULL){
	                        	puerto_escucha_planificador=config_get_int_value(config, "PORT_PLANIFICADOR");
	                        	ip_conec_plani=config_get_string_value(config,"IP_PLANIFICADOR");
	                        	puerto_escucha_memoria=config_get_int_value(config, "PORT_MEMORIA");
	                        	ip_conec_memoria=config_get_string_value(config,"IP_MEMORIA");}


   int planificador = conectar_cliente(puerto_escucha_planificador, ip_conec_plani);

   int memoria = conectar_cliente(puerto_escucha_memoria, ip_conec_memoria);
   free ( ip_conec_memoria);

   char* tamanioPath = recibirMensaje( planificador);
   char* path = recibirMensaje( planificador);

   enviarMesaje(memoria, path);
   return ;
}

void procesarCadena(char* cadena){
  char* line = cadena;
  char** substrings = string_split(line, " ");

	if (strcmp( substrings[0] ,"iniciar")==0){
		//aviso a memoria substrings[1] = nroPaginas
		// if (hay lugar para ejecutar mcod)
		// return mProc x -iniciado/fallo
		//y a planificador
		printf("\n\n encontro iniciar\n\n");
	}

	if (strcmp(substrings[0] ,"leer")==0){
		//aviso a memoria que lea substrings[1]
		printf("\n\n encontro leer\n\n");

	}
	if (strcmp(substrings[0] ,"finalizar")==0){
		//aviso a memoria y a planificador
		printf("\n\n encontro finalizar\n\n");
	}
						free(substrings[0]);
	                    //free(substrings)
}


void* abrirmCod(char* path){

	FILE *archivo;
 	char caracteres[100];
 	//caracteres= malloc(sizeof caracteres);

 	archivo = fopen(path,"r");

 				if (archivo == NULL){

 					printf("\nError de apertura del archivo. \n\n");
 			        }else{


 					while (feof(archivo) == 0)
 						{
 							char* cadena = fgets(caracteres,100,archivo);

 							procesarCadena(cadena);


 						}
 			        }

 		//free (caracteres);
        fclose(archivo);
        return EXIT_SUCCESS;
}


#endif
