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
#include <semaphore.h>

pthread_mutex_t mutex;



int recolectar_Texto(char cadena[1500], int punta, char texto[15])
{
	int i =0;

	while (cadena[punta]!='"'){
		texto[i]=cadena[punta];
		punta++; i++;
	}
	texto[i]='\0';
	punta++;

	return punta;
}

int recolectar_paginaEscribir(char cadena[1500], int punta, char pagina[3])
{
	int i =0;

	while (cadena[punta]!=' '){
		pagina[i]=cadena[punta];
		punta++; i++;
	}
	pagina[i]='\0';
	punta++;

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
	if(!strcmp(comando, "escribir")){instruccion=2;}
	if(!strcmp(comando, "entrada-salida")){instruccion=3;}
	if(!strcmp(comando, "finalizar")){instruccion=4;}

	return instruccion;
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

int procesar_instruccion(char* cadena,char comando[15],int punta,char pagina[3], int memoria, int planificador,t_log* logger, PCB* PcbAux, int retardo,char texto[20])
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

			log_info(logger, "mProc %s Iniciado", PcbAux->nombreProc);
			log_info(logger, "El pid es %d", PcbAux->PID);

			t_msgHeader header;
			memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
			header.msgtype = 2;
			header.payload_size = PcbAux->PID; //en este caso el playload lo usamos para pid
			send(planificador, &header, sizeof( t_msgHeader), 0);

			PcbAux->contadorProgram ++;
			printf("El contador de programa es:%d\n", PcbAux->contadorProgram);
			sleep(retardo);

		}else{
			printf("No hay lugar\n");
			printf("mProc %s Fallo\n", PcbAux->nombreProc);
			log_info(logger, "mProc %s Fallo\n", PcbAux->nombreProc);
			PcbAux->contadorProgram = 0;
			punta=1501;
			sleep(retardo);
		}
		break;
	}
	case 1://Leer
	{
		instruccion = 2;
		punta=recolectar_pagina(cadena,punta,pagina) + 1;
		int paginas = atoi(pagina);
		printf("Encontro leer\n");
		enviarSolicitud (PcbAux->PID, instruccion, paginas , memoria);
		PROCESO msj = recibirMsjMemoria(memoria);

		printf("mensaje de la memoria %d \n",msj.aceptado);

		if(msj.aceptado==1)
		{
			printf("Pude leer\n");
			/*char* mensaje;
			mensaje = malloc(msj.tamanioMensaje);
			recv(memoria, mensaje, msj.tamanioMensaje, 0);
			printf("El contenido leido es:%s\n", mensaje);*/

			log_info(logger, "El pid es %d", PcbAux->PID);
			log_info(logger, "mProc %s - Pagina %d leida: contenido", PcbAux->nombreProc, paginas);


			PcbAux->contadorProgram++;
			sleep(retardo);

		}else{
			printf("No pudo leer\n");
			log_info(logger, "El pid es %d", PcbAux->PID);
			log_info(logger, "mProc %s Fallo lectura",PcbAux->nombreProc);
			printf("mProc %s Fallo\n", PcbAux->nombreProc);
			sleep(retardo);
			punta=15001;
		}
		break;
	}
	case 2://Escribir
	{
		printf("Encontro Escribir\n");
		instruccion = 3;
		int aux=recolectar_paginaEscribir(cadena,punta,pagina);
		punta=aux;
		int paginas = atoi(pagina);
		aux= recolectar_Texto(cadena, punta, texto);
		punta=aux+2;

		printf("El comando es Escribir en la pagina: %d \n", paginas);

		enviarSolicitud (PcbAux->PID, instruccion, paginas , memoria);//MANDAR TEXTO
		PROCESO msj = recibirMsjMemoria(memoria);

		printf("El texto a escribir es: %s \n",texto);
		printf("mensaje de la memoria %d \n",msj.aceptado);

		if(msj.aceptado==1){
			printf("mProc %s - Pagina %d escrita: %s \n", PcbAux->nombreProc,paginas, texto);
			PcbAux->contadorProgram++;
			//eviar a memoria el contenido a escribir
			/*char* mensaje;
					mensaje = malloc(sizeof(texto));
					send(memoria, mensaje, sizeof(texto), 0);
					printf("El contenido leido es:%s\n", mensaje);*/
			log_info(logger, "El pid es %d", PcbAux->PID);
			log_info(logger, "mProc %s - Pagina %d escrita:%s",PcbAux->nombreProc, paginas, texto);

			printf("El contador de programa es:%d\n", PcbAux->contadorProgram);
		}else{
			printf("La escritura fallo");
			log_info(logger, "El pid es %d", PcbAux->PID);
			log_info(logger, "mProc %s Fallo lectura",PcbAux->nombreProc);
		}
		break;
	}
	case 3://Entrada-salida
	{
		printf("Encontro entrada-salida\n");
		instruccion = 5;

		int aux=recolectar_pagina(cadena,punta,pagina);
		punta=aux+1;
		int tiempo = atoi(pagina);
		printf("El comando es Entrada-Salida y la cantidad de tiempo es %d \n", tiempo);

		/*enviarSolicitud (PcbAux->PID, instruccion, tiempo , memoria);
		PROCESO msj = recibirMsjMemoria(memoria);
		printf("El mensaje de la memoria es: %d", msj.aceptado);*/
		PcbAux->contadorProgram++;

		t_msgHeader header;
		memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
		header.msgtype = 4;
		header.payload_size = 0; //en este caso el playload lo usamos para pid
		send(planificador, &header, sizeof( t_msgHeader), 0);

		PCB_PARCIAL parcial;
		memset(&parcial, 0, sizeof(PCB_PARCIAL)); // Ahora el struct tiene cero en todos sus miembros
		parcial.contadorDePrograma= PcbAux->contadorProgram;
		parcial.pid= PcbAux->PID;
		parcial.tiempo= tiempo;
		send(planificador, &parcial, sizeof( PCB_PARCIAL), 0);
		//DEJAR DE LLERRRR !!!!!!!!!!!!!!!!!!!!!!!!
		printf("mProc %s en entrada-salida de tiempo %d\n", PcbAux->nombreProc,tiempo);
		printf("El contador de programa es:%d\n", PcbAux->contadorProgram);
		log_info(logger, "El pid es %d", PcbAux->PID);
		log_info(logger, "mProc %s en entrada-salida de tiempo %d",PcbAux->nombreProc, tiempo);
		punta=1501;
		break;
	}
	case 4://Finalizar
	{
		instruccion = 3;
		printf("Encontre Finalizar\n");
		punta+=200;

		enviarSolicitud (PcbAux->PID, instruccion, 0 , memoria);
		PROCESO msj = recibirMsjMemoria(memoria);
		printf("mensaje de la memoria %d \n",msj.aceptado);

		if(msj.aceptado==1)
		{
			printf("El proceso Pudo finalizar\n");
			log_info(logger, "El pid es %d", PcbAux->PID);
			log_info(logger, "mProc %s finalizado", PcbAux->nombreProc);
			PcbAux->contadorProgram++;
			t_msgHeader header;
			memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
			header.msgtype = 3;
			header.payload_size = 0; //en este caso el playload lo usamos para pid
			send(planificador, &header, sizeof( t_msgHeader), 0);

			PCB_PARCIAL parcial;
			memset(&parcial, 0, sizeof(PCB_PARCIAL)); // Ahora el struct tiene cero en todos sus miembros
			parcial.pid = PcbAux->PID; //en este caso el playload lo usamos para pid
			parcial.tiempo = 0;
			parcial.contadorDePrograma = PcbAux->contadorProgram;
			send(planificador, &parcial, sizeof( PCB_PARCIAL), 0);

			printf("El contador de programa es:%d\n", PcbAux->contadorProgram);
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
int procesarCadena(char cadena[1500], int memoria, int planificador,t_log* logger, PCB* PcbAux, int retardo){
	int valor=1;
	int punta=0;
	int tamBuff=strlen(cadena);
	char comando[15];
	char pagina[3];
	char texto[20];
	/*PROCESO msj;
	msj.aceptado=1;*/
	while(punta<(tamBuff-1))
	{
		int aux;
		aux= recolectar_instruccion(cadena,comando, punta);
		punta=aux;
		//printf("La punta es %d, y la instruccion es: %s \n", punta,comando);
		aux =procesar_instruccion(cadena, comando, punta, pagina, memoria, planificador,  logger,PcbAux, retardo, texto);
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
	//printf("El tamaño del mCod es:%d \n", len);
	//char* buffer = malloc(len+10);
	fseek(fp, 0, SEEK_SET);
	if (len != fread(buffer, sizeof(char), len+10, fp))
	{
		free(buffer);

	}

	fclose(fp);
}

typedef struct {
	int puerto_escucha_planificador;
	char* ip_conec_plani;
	int puerto_escucha_memoria;
	char* ip_conec_memoria;
	t_log* logger;
	int retardo;

}struct1;
//pthread_t id = pthread_self();
void* conectar(void* mensa){
	 struct1 *b;
	 b=(struct1*)mensa;

	int puertoPlanificador = b->puerto_escucha_planificador;
	char* ipPlanificador = b ->ip_conec_plani;
	int puertoMemoria = b-> puerto_escucha_memoria;
	char* ipMemoria = b->ip_conec_memoria;
	t_log* logger = b->logger;
	int retardo = b->retardo;



	printf("El ID de hilito es:%u\n", (unsigned int)pthread_self());


	pthread_mutex_lock(&mutex);
	int planificador = conectar_cliente(puertoPlanificador, ipPlanificador);
	int memoria = conectar_cliente(puertoMemoria, ipMemoria);


	if (memoria > 0){
		log_info(logger, "El hilo %u se conecto a memoria correctamente",(unsigned int)pthread_self());
	}else {
			log_info(logger, "error al conectarse con memoria");
		}
	printf("El planificador es %d\n", planificador);
	pthread_mutex_unlock(&mutex);

	char* mensaje;
	mensaje=malloc(100);
	recv(memoria, mensaje, 100, 0);
	char* aux = recibirMensaje(planificador, logger);
	free(aux);
	while(1){

		t_msgHeader header2;
		memset(&header2, 0, sizeof(t_msgHeader));
		header2.msgtype = 0; //significa estoy libre
		header2.payload_size = planificador;

		send(planificador, &header2, sizeof( t_msgHeader), 0);


		char* buffer;
		PCB *PcbAux =malloc(sizeof(PCB));
		t_msgHeader header;
		memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
		printf("El planificador es %d\n", planificador);
		recv(planificador, &header, sizeof( t_msgHeader), 0);
		//printf("El tamaño delmensaje  es: %d\n\n",header.payload_size);

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
		offset+=strlen(PcbAux->nombreProc)+1;
		memcpy(&PcbAux->quantum,buffer +offset  ,  sizeof(int));

		printf("El mensaje del planificador PCB es:\n");
		printf("Este es el PID:                  %d\n", PcbAux->PID);
		printf("Este es el contador de programa: %d\n",PcbAux->contadorProgram);
		printf("La CPU asignada es:              %d\n",PcbAux->cpu_asignada);
		printf("El nombre  del proceso es:       %s\n", PcbAux->nombreProc);
		printf("El path es:                      %s\n", PcbAux->path);
		printf("El quantum es:                   %d\n", PcbAux->quantum);

		log_info(logger, "----------------PCB-------------------------------------------------------------------------------------------------------------------------------------------------------------");
		log_info(logger, "Este es el PID:                  %d\n", PcbAux->PID);
		log_info(logger, "Este es el contador de programa: %d\n",PcbAux->contadorProgram);
		log_info(logger, "La CPU asignada es:              %d\n",PcbAux->cpu_asignada);
		log_info(logger, "El nombre  del proceso es:       %s\n", PcbAux->nombreProc);
		log_info(logger, "El path es:                      %s\n", PcbAux->path);
		log_info(logger, "El quantum es:                   %d\n", PcbAux->quantum);
		log_info(logger, "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");


		// si el quantum es -1 la planificacion es fifo, sino es round robin
		//contadorProgram; si es -1 no abrimos, ejecutamos solo finalizar

		if(PcbAux->contadorProgram!= -1){
			char archivoProc[1500];
			memset(archivoProc,'\0', 1500);
			abrir(PcbAux,archivoProc);

			if(PcbAux->quantum == 0){
				//fifo
				procesarCadena(archivoProc, memoria, planificador,logger, PcbAux, retardo);

			}else{
				procesarCadenaConQuantum(PcbAux->quantum, archivoProc, memoria, planificador, logger, PcbAux, retardo);
			}
		}else{
			sentenciaFinalizar(memoria, planificador, logger, PcbAux, retardo);
		}

		free(buffer);
	}
	return EXIT_SUCCESS;
}



int procesarCadenaConQuantum(int quantum , char cadena[1500], int memoria, int planificador,t_log* logger, PCB* PcbAux, int retardo){
	int valor=1;
	int punta=0;
	char comando[15];
	char pagina[3];
	char texto[20];
	/*PROCESO msj;
	msj.aceptado=1;*/

	//Ejecución de ráfaga concluída, indicando PID.
	int i=0;
	while((strcmp(comando, "finalizar")) || quantum>=i )
	{
		int aux;
		aux= recolectar_instruccion(cadena,comando, punta);
		punta=aux;
		//printf("La punta es %d, y la instruccion es: %s \n", punta,comando);
		aux =procesar_instruccion(cadena, comando, punta, pagina, memoria, planificador,  logger,PcbAux, retardo, texto);
		punta=aux;
		i++;
	}
	if(strcmp(comando, "finalizar")){
		t_msgHeader header;
		memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
		header.msgtype = 5;
		header.payload_size = PcbAux->PID; //en este caso el playload lo usamos para pid
		send(planificador, &header, sizeof( t_msgHeader), 0);

		PCB_PARCIAL parcial;
		memset(&parcial, 0, sizeof(PCB_PARCIAL)); // Ahora el struct tiene cero en todos sus miembros
		parcial.pid = PcbAux->PID; //en este caso el playload lo usamos para pid
		parcial.tiempo = 0;
		parcial.contadorDePrograma = PcbAux->contadorProgram;
		send(planificador, &parcial, sizeof( PCB_PARCIAL), 0);
	}
	log_info(logger, "El pid es %d", PcbAux->PID);
	log_info(logger, "mProc %s finalizo el quantum de %d", PcbAux->nombreProc, quantum);
	return valor;
}
void sentenciaFinalizar(int memoria, int planificador,t_log* logger, PCB* PcbAux, int retardo){
	int instruccion = 4;
	printf("Encontre Finalizar\n");
	//int punta=200;
	//Ejecución de ráfaga concluída, indicando PID.
	enviarSolicitud (PcbAux->PID, instruccion, 0 , memoria);
	PROCESO msj = recibirMsjMemoria(memoria);
	printf("mensaje de la memoria %d \n",msj.aceptado);

	if(msj.aceptado==1)
	{
		printf("El proceso Pudo finalizar\n");
		log_info(logger, "El pid es %d", PcbAux->PID);
		log_info(logger, "mProc %s solo ejecuta sentencia finalizado", PcbAux->nombreProc);

		t_msgHeader header;
		memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
		header.msgtype = 3;
		header.payload_size = 0; //en este caso el playload lo usamos para pid
		send(planificador, &header, sizeof( t_msgHeader), 0);

		PCB_PARCIAL parcial;
		memset(&parcial, 0, sizeof(PCB_PARCIAL)); // Ahora el struct tiene cero en todos sus miembros
		parcial.pid = PcbAux->PID; //en este caso el playload lo usamos para pid
		parcial.tiempo = 0;
		parcial.contadorDePrograma = PcbAux->contadorProgram;
		send(planificador, &parcial, sizeof( PCB_PARCIAL), 0);
		sleep(retardo);

	}

}
