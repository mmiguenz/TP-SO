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
#include <errno.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <commons/string.h>
#include "cliente.h"
#include <assert.h>
#include "hilito.h"

struct param{
	int puerto_escucha_planificador;
	char* ip_conec_plani;
	int puerto_escucha_memoria;
	char* ip_conec_memoria;
};
void* conectar(struct param *mensa){

	int puertoPlanificador = mensa->puerto_escucha_planificador;
	char* ipPlanificador = mensa ->ip_conec_plani;
	int puertoMemoria = mensa-> puerto_escucha_memoria;
	char* ipMemoria = mensa->ip_conec_memoria;

	int planificador = conectar_cliente(puertoPlanificador, ipPlanificador);

   int memoria = conectar_cliente(puertoMemoria, ipMemoria);

   char* mensaje;

  // while(1){
   mensaje = (char*)malloc(sizeof(mensaje));
   mensaje = "estoy libre";

   recibirMensaje(planificador);
   enviarMesaje(planificador, "estoy libre");

   char* mCod;
   mCod = (char*)malloc(sizeof(mCod));
   //mCod = recibirMensaje(planificador);
   //aca en un futuro recibiremos un paquete

   printf("\n\n encontro iniciar\n\n %s \n", recibirMensaje(planificador));
   //abrirmCod(mCod, memoria, planificador);

   //}
   return EXIT_SUCCESS;
}

int procesarCadena(char* cadena, int memoria, int planificador){

	char* line = cadena;
	char** substrings = string_split(line, " ");

			if (strcmp( substrings[0] ,"iniciar")==0){

				printf("\n\n encontro iniciar\n\n");

				char nroPaginas = substrings[1] ;
				enviarMesaje(memoria, nroPaginas);

				char* msj = recibirMensaje(memoria);
				enviarMesaje(planificador, msj);
				msj = recibirMensaje(planificador);

						if (strcmp (msj ,"continuar")==0){
									return 1 ;
								}else {return 0;}
				} else if (strcmp ( substrings[0] ,"leer")==0){
						//aviso a memoria que lea substrings[1]
						printf("\n\n encontro leer\n\n");

						char nroPaginas = substrings[1] ;
						enviarMesaje(memoria, nroPaginas);

						char* msj = recibirMensaje(memoria);
						enviarMesaje(planificador, msj);
						msj = recibirMensaje(planificador);
							if (strcmp (msj ,"continuar")==0){
								return 1 ;
							}else {return 0;}

					}else if (strcmp(substrings[0] ,"finalizar")==0){
							//aviso a memoria y a planificador
							printf("\n\n encontro finalizar\n\n");

							char FinalizarmCod = substrings[1] ;
							enviarMesaje(memoria, FinalizarmCod);

							char* msj = recibirMensaje(memoria);
							enviarMesaje(planificador, msj);
							msj = recibirMensaje(planificador);
							return 2;}
			return 2;
							free(substrings[0]);
	                    //free(substrings)
}


void abrirmCod(char* path, int memoria, int planificador){

	FILE *archivo;
 	char caracteres[100];
 	//caracteres= malloc(sizeof caracteres);

 	archivo = fopen(path,"r");

 				if (archivo == NULL){

 					printf("\nError de apertura del archivo. \n\n");
 			        }else{
 			        	int valor = 1;

 					while (feof(archivo) == 0 || valor ==  1)
 						{
 							char* cadena = fgets(caracteres,100,archivo);

 							 valor = procesarCadena(cadena, memoria, planificador);


 						}
 			        }

 		//free (caracteres);
        fclose(archivo);
        return ;
}


#endif
