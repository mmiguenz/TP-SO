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



int procesarCadena(char cadena[1500], int memoria, int planificador,t_log* logger, PCB* PcbAux, int retardo){

	int valor=1;

	int punta=0;

	int tamBuff=strlen(cadena);
	char comando[15];
	char pagina[3];

	while(punta<(tamBuff-1))
	{
		int aux;
	aux= recolectar_instruccion(cadena,comando, punta);
	punta=aux;
	printf("La punta es %d, y la instruccion es: %s \n", punta,comando);
	aux =procesar_instruccion(cadena, comando, punta, pagina, memoria, planificador,  logger,PcbAux, retardo);
	punta=aux;
	}
return valor;
}



void abrir(PCB* PcbAux, char buffer[1500])
{
		FILE * fp;
		fp = fopen(PcbAux->path, "r");
		int len;
		if( fp == NULL )
		   {
		      perror ("Error opening file");
		    }
		 fseek(fp, 0, SEEK_END);
		 len = ftell(fp);
		 printf("El tamaño del mCod es:%d \n", len);
		 //char* buffer = malloc(len+10);
		 fseek(fp, 0, SEEK_SET);
		 if (len != fread(buffer, sizeof(char), len+10, fp))
		   	{
		   		free(buffer);

		   	}

	fclose(fp);

	 strcat(buffer,"\n finalizar;");

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

      while(1){
	char* aux = recibirMensaje(planificador, logger);
    free(aux);

    t_msgHeader header2;
    memset(&header2, 0, sizeof(t_msgHeader));
    header2.msgtype = 0; //significa estoy libre
    header2.payload_size = planificador;

    send(planificador, &header2, sizeof( t_msgHeader), 0);



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
    char archivoProc[1500];
    abrir(PcbAux,archivoProc);

    printf("El archivo es: \n %s",archivoProc);
    procesarCadena(archivoProc, memoria, planificador,logger, PcbAux, retardo);

	 free(buffer);
}

    return EXIT_SUCCESS;
}





int recolectar_instruccion(char* cadena,char comando[15],int punta)
{
	 int i =0;

	    while ((cadena[punta]!=' ') & (cadena[punta]!=';')){

	    comando[i]=cadena[punta];
	    i++;
	    punta++;
	    }
	    comando[i]='\0';
	    return punta;
	}

int procesar_instruccion(char* cadena,char comando[15],int punta,char pagina[3], int memoria, int planificador,t_log* logger, PCB* PcbAux, int retardo)
{
	int instruccion;

	switch(identificar_instruccion(comando))
	{
	case 0://Iniciar
	{
		instruccion = 1;

		int aux=recolectar_pagina(cadena,punta,pagina);
		punta=aux+1;
		int paginas = atoi(pagina);

		printf("El comando es Iniciar y la cantidad de paginas es %d \n", paginas);


		enviarSolicitud (PcbAux->PID, instruccion, paginas , memoria);

		PROCESO msj = recibirMsjMemoria(memoria);

		printf("mensaje de la memoria %d \n",msj.aceptado);

		if(msj.aceptado==1)
		{
			printf("Hay lugar\n");
			//log_info(logger, "Hay lugar para: %s", PcbAux->nombreProc);
			//log_info(logger, "El pid es %s", PcbAux->PID);
			//log_info(logger, "mProc %s Iniciado", PcbAux->nombreProc);
			//log_info(logger, "Cantidad de paginas %s", substrings[1]);
			t_msgHeader header;
			memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
			header.msgtype = 2;
			header.payload_size = PcbAux->PID; //en este caso el playload lo usamos para pid
			send(planificador, &header, sizeof( t_msgHeader), 0);
			sleep(retardo);

		}




		break;
	}
	case 1://Leer
		{
		instruccion = 2;
		punta=recolectar_pagina(cadena,punta,pagina) + 1;
		int paginas = atoi(pagina);

		enviarSolicitud (PcbAux->PID, instruccion, paginas , memoria);

		PROCESO msj = recibirMsjMemoria(memoria);

		printf("mensaje de la memoria %d \n",msj.aceptado);

		if(msj.aceptado==1)
		{
			printf("Pude leer\n");
			//log_info(logger, "Hay lugar para: %s", PcbAux->nombreProc);
			//log_info(logger, "El pid es %s", PcbAux->PID);
			//log_info(logger, "mProc %s Iniciado", PcbAux->nombreProc);
			//log_info(logger, "Cantidad de paginas %s", substrings[1]);
			t_msgHeader header;
			memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
			header.msgtype = 2;
			header.payload_size = PcbAux->PID; //en este caso el playload lo usamos para pid
			send(planificador, &header, sizeof( t_msgHeader), 0);
			sleep(retardo);

		}
		break;
	}
	case 2://Finalizar
	{
		instruccion = 3;
		printf("Encontre Finalizar");
		punta+=200;
		enviarSolicitud (PcbAux->PID, instruccion, 0 , memoria);

				PROCESO msj = recibirMsjMemoria(memoria);

				printf("mensaje de la memoria %d \n",msj.aceptado);

				if(msj.aceptado==1)
				{
					printf("El proceso Pudo finalizar\n");
					//log_info(logger, "Hay lugar para: %s", PcbAux->nombreProc);
					//log_info(logger, "El pid es %s", PcbAux->PID);
					//log_info(logger, "mProc %s Iniciado", PcbAux->nombreProc);
					//log_info(logger, "Cantidad de paginas %s", substrings[1]);
					t_msgHeader header;
					memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
					header.msgtype = 2;
					header.payload_size = PcbAux->PID; //en este caso el playload lo usamos para pid
					send(planificador, &header, sizeof( t_msgHeader), 0);
					sleep(retardo);

				}


		break;

	}
	case -1://Error
		{
			printf("Error de sentencias!!!");
			punta++;
			break;
		}
	}
	return punta;
}



int recolectar_pagina(char cadena[1500], int punta, char pagina[3])
{
	   int i =0;

		        while (cadena[punta]!=';'){
		        pagina[i]=cadena[punta];
		        punta++; i++;
		        }
		        pagina[i]='\0';
		        punta++;

	return punta;
}


int identificar_instruccion(char comando[15])
{
	int instruccion = -1;

	if(!strcmp(comando, "iniciar")){instruccion=0;}
	if(!strcmp(comando, "leer")){instruccion=1;}
	if(!strcmp(comando, "finalizar")){instruccion=2;}

	return instruccion;


	}
