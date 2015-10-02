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
	int retardo;
};



int procesarCadena(char* cadena, int memoria, int planificador,t_log* logger, PCB* PcbAux, int retardo){
	//char* line = cadena;
	//line = (char*)malloc(sizeof(char*));
	char** substrings =malloc(sizeof(char**));
	char** substrings2 =malloc(sizeof(char**)); // NO PARA ESCRIBIR!!!!!!!!!!
	substrings = string_split(cadena, " ");
	substrings2 = string_split(substrings[1], ";");
	int valor;
	int instruccion;

	if (strcmp( substrings[0] ,"iniciar")==0){
		printf("Encontro en %s Iniciado Cantidad de paginas %s\n\n", PcbAux->nombreProc, substrings[1]);
		instruccion = 1;

		int pagina = atoi(substrings2[0]);

		enviarSolicitud (PcbAux->PID, instruccion, pagina , memoria);
		PROCESO* msj = recibirMsjMemoria(memoria);

		printf("mensaje de la memoria %d",msj->aceptado);
		if (msj->aceptado == 1){
			printf("Hay lugar\n");
			log_info(logger, "Hay lugar para: %s", PcbAux->nombreProc);
			log_info(logger, "mProc %s Iniciado", PcbAux->nombreProc);
			log_info(logger, "Cantidad de paginas %s", substrings[1]);
			valor = 1;
			t_msgHeader header;
			memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
			header.msgtype = 2;
			header.payload_size = PcbAux->PID; //en este caso el playload lo usamos para pid
			send(planificador, &header, sizeof( t_msgHeader), 0);
			sleep(retardo);
		}else{
			printf("No hay lugar\n");
			printf("mProc %s Fallo\n", PcbAux->nombreProc);
			log_info(logger, "mProc %s Fallo\n", PcbAux->nombreProc);
			//aviso a planificador que fallo
			valor = 0;
			sleep(retardo);

		}free(msj);
	} else if (strcmp ( substrings[0] ,"leer")==0){
				printf("Encontro en mProc %s Pagina %s leer\n", PcbAux->nombreProc,substrings[1]);
				instruccion = 2;
				int pagina = atoi(substrings2[0]);

				enviarSolicitud (PcbAux->PID, instruccion, pagina , memoria);
				PROCESO* msj = recibirMsjMemoria(memoria);

				printf("mensaje de memoria %d",msj->aceptado);
				if (msj->aceptado == 1){
					log_info(logger, "mProc %s comienza lectura\n", PcbAux->nombreProc);
					log_info(logger, "En pagina %s\n", substrings[1]);
					printf("pudo leer\n");
					valor = 1;
					free(msj);
					sleep(retardo);
				}else{
					printf("No pudo leer\n");
					log_info(logger, "mProc %s Fallo\n",PcbAux->nombreProc);
					printf("mProc %s Fallo\n", PcbAux->nombreProc);
					valor = 0;
					sleep(retardo);
				}

	}else if (strcmp(substrings[0] ,"finalizar")==0){
				instruccion = 5;
				printf("mProc %s Finalizado\n", PcbAux->nombreProc);
				log_info(logger, "mProc %s Finalizado\n", PcbAux->nombreProc);
				int pagina = atoi(substrings[1]);

				enviarSolicitud (PcbAux->PID, instruccion, pagina, memoria);
				PROCESO* msj = recibirMsjMemoria(memoria);
				valor = 1;
				free(msj);
				t_msgHeader header;
				memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
				header.msgtype = 3;
				header.payload_size = PcbAux->PID; //en este caso el playload lo usamos para pid
				send(planificador, &header, sizeof( t_msgHeader), 0);
				sleep(retardo);
	}

string_iterate_lines(substrings,(void*) free);
string_iterate_lines(substrings2,(void*) free);
free(substrings);
return valor;
}



void abrir(PCB* PcbAux, int memoria, int planificador, t_log* logger, int retardo) {
	char *cadena = malloc(sizeof(char*));
	FILE * fp;
	int valor = 1;
	fp = fopen(PcbAux->path, "r");
	if (fp == NULL) {
		printf("\nError de apertura del archivo. \n\n");
	} else {
		while ((fgets(cadena, 100, fp) != NULL) && (valor == 1)) {
			//printf("%s\n",cadena);
			valor = procesarCadena(cadena, memoria, planificador, logger, PcbAux, retardo);
		}
		fclose(fp);
	}

	free(cadena);
	return;
}


void* conectar(struct param *mensa){

	int puertoPlanificador = mensa->puerto_escucha_planificador;
	char* ipPlanificador = mensa ->ip_conec_plani;
	int puertoMemoria = mensa-> puerto_escucha_memoria;
	char* ipMemoria = mensa->ip_conec_memoria;
	t_log* logger = mensa->logger;
	int retardo = mensa->retardo;
	int planificador = conectar_cliente(puertoPlanificador, ipPlanificador);
	int memoria = conectar_cliente(puertoMemoria, ipMemoria);
	log_info(logger, "me conecte con memoria");

	char* mensaje;
    //  while(1){

    //mensaje = "estoy libre\n";

    char* aux = recibirMensaje(planificador, logger);
    free(aux);

    t_msgHeader header2;
    memset(&header2, 0, sizeof(t_msgHeader));
    header2.msgtype = 0; //significa estoy libre
    header2.payload_size = planificador;

    send(planificador, &header2, sizeof( t_msgHeader), 0);

    //enviarMesaje(memoria, mensaje, logger);

    char* buffer;
    PCB *PcbAux =malloc(sizeof(PCB));
    t_msgHeader header;
    memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
    recv(planificador, &header, sizeof( t_msgHeader), 0);
    printf("El tamaño delmensaje  es: %d\n\n",header.payload_size);

    buffer=malloc(header.payload_size+5);
    recv(planificador, buffer, header.payload_size, 0);
    int offset=0;
    memcpy(&PcbAux->PID,buffer +offset  ,  sizeof(int));
    offset+=sizeof(int);
    memcpy(&PcbAux->contadorProgram,buffer +offset, sizeof(int));
    offset+=sizeof(int);
    memcpy(&PcbAux->cpu_asignada,buffer +offset  ,  sizeof(int));
    offset+=sizeof(int);
    PcbAux->path=strdup(buffer+offset);
    offset+=strlen(PcbAux->path)+1;
    PcbAux->nombreProc=strdup(buffer +offset);


    printf("este es el pid  %d\n", PcbAux->PID);
    printf("este es el contador %d\n",PcbAux->contadorProgram);
    printf("cpu %d\n",PcbAux->cpu_asignada);
    printf("el nombre  del proceso es %s\n", PcbAux->nombreProc);
    printf("El path %s\n", PcbAux->path);

    abrir(PcbAux, memoria, planificador, logger, retardo);

/*
    char** substring1 = string_split(mCod,"$");
    char** substring2 = string_split(mCod,"/");




    string_iterate_lines(substring1, free);
    string_iterate_lines(substring2, free);
    free(mCod);
    free(substring1);
    free(substring2);

*/
    free(buffer);
    return EXIT_SUCCESS;
}

