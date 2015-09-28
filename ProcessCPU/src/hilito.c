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
#include <commons/log.h>

struct param{
	int puerto_escucha_planificador;
	char* ip_conec_plani;
	int puerto_escucha_memoria;
	char* ip_conec_memoria;
	t_log* logger;
};



int procesarCadena(char* cadena, int memoria, int planificador,t_log* logger, char* nombreProc){

	char* line = cadena;
	//line = (char*)malloc(sizeof(char*));

	char** substrings =malloc(sizeof(char**));
	substrings = string_split(line, " ");

	int valor;

	if (strcmp( substrings[0] ,"iniciar")==0){
				printf("mProc %s Iniciado\n\n", nombreProc);
				printf("Cantidad de paginas %s",substrings[1]);

				enviarMesaje(memoria, "encontre iniciar\n\n", logger);
				enviarMesaje(memoria, substrings[1], logger);

				//log_info(logger, "mProc %s Iniciado", nombreProc);
				//log_info(logger, "Cantidad de paginas %s", substrings[1]);

				int msj;
				msj = atoi(recibirMensaje(memoria, logger));
					if (msj == 1){
						//enviarMesaje(planificador, "hay lugar");
						printf("Hay lugar\n");
						//deberia mandarme donde lo guardo
					//	log_info(logger, "Hay lugar para: %s", nombreProc);
						valor = 1;
					}else{
						printf("No hay lugar\n");
						printf("mProc %s Fallo\n", nombreProc);
						//log_info(logger, "mProc %s Fallo\n", nombreProc);
						valor = 0;
					}
	} else if (strcmp ( substrings[0] ,"leer")==0){
				printf("mProc %s Pagina %s leida: contenido\n", nombreProc,substrings[1]);

				enviarMesaje(memoria, "encontre leer\n", logger);
				enviarMesaje(memoria, substrings[1], logger);

				//log_info(logger, "mProc %s comienza lectura\n", nombreProc);
			//	log_info(logger, "Pagina %s\n", substrings[1]);

				int msj = atoi(recibirMensaje(memoria, logger));

				if (msj == 1){
					//enviarMesaje(planificador, "lei");
					printf("pudo leer\n");
					//log_info(logger, "mProc %s pudo leer\n", nombreProc);
					valor = 1;

				}else{
					printf("No pudo leer\n");
					//log_info(logger, "mProc %s Fallo\n", nombreProc);
					printf("mProc %s Fallo\n", nombreProc);
					valor = 0;
				}

	}else if (strcmp(substrings[0] ,"finalizar")==0){
				printf("mProc %s Finalizado\n", nombreProc);

				//log_info(logger, "mProc %s Finalizado\n", nombreProc);

				enviarMesaje(memoria, "Finaliza mProc\n", logger);
				valor = 1;

	}

	free(substrings);
	//free(line);
	return valor;
}



void abrir(char* path, int memoria, int planificador, t_log* logger,
		char* nombreProc) {
	char *cadena = (char*) malloc(sizeof(char*));
	FILE * fp;
	int valor = 1;
	fp = fopen(path, "r");
	if (fp == NULL) {
		printf("\nError de apertura del archivo. \n\n");
	} else {
		while ((fgets(cadena, 100, fp) != NULL) && (valor == 1)) {
			printf("%s\n",cadena);
			//valor = procesarCadena(cadena, memoria, planificador, logger,
				//	nombreProc);
		}
		fclose(fp);
	}

	free(cadena);
}


void* conectar(struct param *mensa){

	int puertoPlanificador = mensa->puerto_escucha_planificador;
	char* ipPlanificador = mensa ->ip_conec_plani;
	int puertoMemoria = mensa-> puerto_escucha_memoria;
	char* ipMemoria = mensa->ip_conec_memoria;
	t_log* logger = mensa->logger;
	int planificador = conectar_cliente(puertoPlanificador, ipPlanificador);
	int memoria = conectar_cliente(puertoMemoria, ipMemoria);



	char* mensaje;
    //  while(1){
    mensaje = "estoy libre\n";
    recibirMensaje(planificador, logger);
    enviarMesaje(planificador, mensaje, logger);
    enviarMesaje(memoria, mensaje, logger);
    //recibirMensaje(memoria, logger);


    char* mCod = recibirMensaje(planificador, logger);
    char** substring1 = string_split(mCod,"$");
    char** substring2 = string_split(mCod,"/");


    abrir(substring1[0], memoria, planificador, logger, substring2[5]);

    string_iterate_lines(substring1, free);
    string_iterate_lines(substring2, free);
    free(mCod);
    free(substring1);
    free(substring2);


    return EXIT_SUCCESS;
}

