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



int procesarCadena(char* cadena, int memoria, int planificador){
	char* line = cadena;
	char** substrings = string_split(line, " ");
    int valor;
	if (strcmp( substrings[0] ,"iniciar")==0){
				printf("\n\n mProc X Iniciado\n\n");
				//printf("\n\n mProc X Fallo\n\n");
				printf("Cantidad de paginas %s",substrings[1]);
				 valor = 1;
	} else if (strcmp ( substrings[0] ,"leer")==0){
				printf("\n\n mProc X Pagina N leida: contenido\n\n");
				valor = 1;
	}else if (strcmp(substrings[0] ,"finalizar")==0){
				printf("\n\n mProc X Finalizado\n\n");
				valor =1;
	}
	//free(substrings[0]);
	return valor;
}



void abrir(char* path){
	printf("%s  \n \n \n", path);//fanatica de \n


    char caracteres[100];

    FILE * fp;

       fp = fopen ("file.txt", "r");

    if (fp == NULL){
		printf("\nError de apertura del archivo. \n\n");
 	}else{
 		while (feof(fp) == 0){
 				char* cadena = fgets(caracteres,100,fp);
 				printf("%s", cadena);
		}
      }
    fclose(fp);
}


void* conectar(struct param *mensa){

	int puertoPlanificador = mensa->puerto_escucha_planificador;
	char* ipPlanificador = mensa ->ip_conec_plani;
	int planificador = conectar_cliente(puertoPlanificador, ipPlanificador);
	char* mensaje;
    //  while(1){
    mensaje = (char*)malloc(sizeof(mensaje));
    mensaje = "estoy libre";
    recibirMensaje(planificador);
    enviarMesaje(planificador, mensaje);
    char* mCod;
    mCod = (char*)malloc(sizeof(mCod));
    mCod = recibirMensaje(planificador);

    printf("%s  \n \n \n", mCod);//fanatica de \n


        char caracteres[100];

        FILE * fp;

           fp =fopen (mCod, "r");

        if (fp == NULL){
    		printf("\nError de apertura del archivo. \n\n");
     	}else{
     		while (feof(fp) == 0){
     				char* cadena = fgets(caracteres,100,fp);
     				printf("%s", cadena);
    		}

          }





     return EXIT_SUCCESS;
}

