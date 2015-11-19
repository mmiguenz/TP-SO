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
#include <time.h>
#include <commons/collections/queue.h>

//--variable global cola de porcentajes
t_queue * porcentajes_CPU;

pthread_mutex_t mutex;
int primLectBuffer = 1;

//--param de los hilos
typedef struct {
	int puerto_escucha_planificador;
	char* ip_conec_plani;
	int puerto_escucha_memoria;
	char* ip_conec_memoria;
	t_log* logger;
	int retardo;
	t_queue * porcentajes_CPU;
}struct1;


void porcentajesCPU(t_queue * porcentajes_CPU){
	int planificador = conectar_cliente(8082, "127.0.0.1");

	while(1){
		char* mensaje;
		mensaje = malloc(100);
		recv(planificador, mensaje, 100, MSG_WAITALL);

		printf("el msj es %s\n",mensaje);
		free(mensaje);
		pthread_mutex_lock(&mutex);
		usoCPU *auxusoCPU=malloc(sizeof(usoCPU));
		int tamanio=queue_size(porcentajes_CPU);
		printf("el tamanio de la cola es %d\n",tamanio);

		while(tamanio!=0){
			auxusoCPU=queue_pop(porcentajes_CPU);
			printf("La cpu %d tiene un porcentaje de:%d\n",  auxusoCPU->cpu, auxusoCPU->porcentaje);
			queue_push(porcentajes_CPU,auxusoCPU);
			tamanio--;
		}
		 pthread_mutex_unlock(&mutex);
	}
}

usoCPU* buscarNodo(int cpu, t_queue * porcentajes_CPU){

	usoCPU* aux=malloc(sizeof(usoCPU));

	pthread_mutex_lock(&mutex);
	int tamanio=queue_size(porcentajes_CPU);

	while(tamanio!=0){
		aux=queue_pop(porcentajes_CPU);
		tamanio--;
		if(aux->cpu==cpu)
		{

			tamanio=0;
		}
		else{
			queue_push(porcentajes_CPU,aux);
		}
	}
	pthread_mutex_unlock(&mutex);
	return aux;
}

int porcentajeDeUso(int diff, int instrucciones, int retardo){

	int porcentaje;

	//--en 60 pulso deberia hacer 60 instruccicones
	//--diff representa los pulsos transcurridos desde que inicio en segundos

	//--descontamos el retardo pasado por el arch de configuracion
	//--entonces las instruccionesTotales que deberia hacer son


	//--regla de 3simple los pulsos que transcurrieron desde que empezo
	//por las instruccion que deberia haber hecho en 60s divido 60
	int cantInstOptima = (diff*retardo);
	if(cantInstOptima == 0){
		porcentaje = 0;
	}else{
	//ese valor de instrucciones representa el 100% calculamos el % para las cant que hizo

	porcentaje = (instrucciones*100)/cantInstOptima;
	}

	return porcentaje;
}

int recolectar_Texto(char cadena[1500], int punta, char texto[20])
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
	punta++;
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



int procesar_instruccion(char* cadena,char comando[15],int punta,char pagina[3], int memoria, int planificador,t_log* logger, PCB* PcbAux, int retardo,char texto[20],time_t comienzo)
{
	char instruccion;
	time_t final;


	switch(identificar_instruccion(comando))
	{
	case 0://Iniciar
	{
		instruccion = 1;

		int aux=recolectar_pagina(cadena,punta,pagina);
		punta=aux+1;
		int paginas = atoi(pagina);
		printf("El comando es Iniciar y la cantidad de paginas es %d \n", paginas);

		//--mensaje para la MEMORIA
		enviarSolicitud (PcbAux->PID, instruccion, paginas , memoria);
		char msj = *(recibirMsjMemoria(memoria));
		printf("mensaje de la memoria %d \n",msj);

		if(msj==1)
		{
			//--la memoria tiene lugar para iniciar el mCod
			printf("Hay lugar\n");

			log_info(logger, "mProc %s Iniciado", PcbAux->nombreProc);
			log_info(logger, "El pid es %d", PcbAux->PID);

			//--enviamos msj solo header a PLANIFICADOR
			t_msgHeader header;
			memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
			header.msgtype = 2;
			header.payload_size = PcbAux->PID; //en este caso el playload lo usamos para pid
			send(planificador, &header, sizeof( t_msgHeader), 0);

			PcbAux->contadorProgram ++;

			usleep((retardo*1000000));

		}else{
			printf("No hay lugar\n");
			printf("mProc %s Fallo\n", PcbAux->nombreProc);
			log_info(logger, "mProc %s Fallo\n", PcbAux->nombreProc);
			PcbAux->contadorProgram = 0;

			t_msgHeader header;
			memset(&header, 0, sizeof(t_msgHeader));
			header.msgtype = 3;
			header.payload_size =PcbAux->PID;
			send(planificador, &header, sizeof( t_msgHeader), 0);

			PCB_PARCIAL parcial;
			memset(&parcial, 0, sizeof(PCB_PARCIAL));
			parcial.pid = PcbAux->PID;
			parcial.tiempo = 0;
			parcial.contadorDePrograma = PcbAux->contadorProgram;
			send(planificador, &parcial, sizeof( PCB_PARCIAL), 0);
			punta=15001;
			strcpy(comando,"fallo");

			usleep((retardo*1000000));
		}
		break;
	}
	case 1://Leer
	{
		instruccion = 2;
		punta=recolectar_pagina(cadena,punta,pagina) + 1;
		int paginas = atoi(pagina);

		printf("Encontro leer\n");

		char* contenidoLeido = NULL;
		contenidoLeido = enviarSolicitudLectura(PcbAux->PID, instruccion, paginas , memoria);

		char msj = (contenidoLeido > 0)?1:0;
		int bytesRecib = strlen(contenidoLeido)+1;

		printf("bytes recibidos :  %d \n",bytesRecib);

		if(msj>0 && (strcmp(contenidoLeido,"-1") != 0))
		{
			//--msj es el tamaño de lo que leyo
			printf("Pude leer\n");
			printf("El contenido leido es:%s\n", contenidoLeido);

			log_info(logger, "El pid es %d", PcbAux->PID);
			log_info(logger, "mProc %s - Pagina %d leida: %s", PcbAux->nombreProc, paginas, contenidoLeido);

			PcbAux->contadorProgram++;

			usleep((retardo*1000000));
			free(contenidoLeido);

		}else{
			printf("No pudo leer\n");
			log_info(logger, "El pid es %d", PcbAux->PID);
			log_info(logger, "mProc %s Fallo lectura",PcbAux->nombreProc);
			printf("mProc %s Fallo\n", PcbAux->nombreProc);

			t_msgHeader header;
			memset(&header, 0, sizeof(t_msgHeader));
			header.msgtype = 3;
			header.payload_size = 0;
			send(planificador, &header, sizeof( t_msgHeader), 0);

			PCB_PARCIAL parcial;
			memset(&parcial, 0, sizeof(PCB_PARCIAL));
			parcial.pid = PcbAux->PID;
			parcial.tiempo = 0;
			parcial.contadorDePrograma = PcbAux->contadorProgram;
			send(planificador, &parcial, sizeof( PCB_PARCIAL), 0);

			punta=15001;

			usleep((retardo*1000000));

		}
		break;
	}
	case 2://Escribir
	{
		printf("Encontro Escribir\n");
		instruccion = 3;

		int aux=recolectar_paginaEscribir(cadena,punta,pagina)+1;
		punta=aux;
		int paginas = atoi(pagina);

		aux= recolectar_Texto(cadena, punta, texto);
		punta=aux+2;

		printf("El comando es Escribir en la pagina: %d \n", paginas);


		char msj= mandarMsjEscribir(memoria, texto, PcbAux->PID, instruccion, paginas);


		printf("El texto a escribir es: %s \n",texto);
		printf("mensaje de la memoria %d \n",msj);

		if(msj==1){
			printf("mProc %s - Pagina %d escrita: %s \n", PcbAux->nombreProc,paginas, texto);

			PcbAux->contadorProgram++;

			log_info(logger, "El pid es %d", PcbAux->PID);
			log_info(logger, "mProc %s - Pagina %d escrita:%s",PcbAux->nombreProc, paginas, texto);

			printf("El contador de programa es:%d\n", PcbAux->contadorProgram);
			usleep((retardo*1000000));
		}else{
			printf("La escritura fallo \n");
			log_info(logger, "El pid es %d", PcbAux->PID);
			log_info(logger, "mProc %s Fallo lectura",PcbAux->nombreProc);

			t_msgHeader header;
			memset(&header, 0, sizeof(t_msgHeader));
			header.msgtype = 3;
			header.payload_size = 0;
			send(planificador, &header, sizeof( t_msgHeader), 0);

			PCB_PARCIAL parcial;
			memset(&parcial, 0, sizeof(PCB_PARCIAL));
			parcial.pid = PcbAux->PID;
			parcial.tiempo = 0;
			parcial.contadorDePrograma = PcbAux->contadorProgram;
			send(planificador, &parcial, sizeof( PCB_PARCIAL), 0);
			punta=1501;
			usleep((retardo*1000000));
		}
		break;
	}
	case 3://Entrada-salida
	{
		printf("Encontro entrada-salida\n");

		int aux=recolectar_pagina(cadena,punta,pagina);
		punta=aux+1;
		int tiempo = atoi(pagina);
		printf("El comando es Entrada-Salida y la cantidad de tiempo es %d \n", tiempo);

		//--calculamos porcentaje de uso
		final = time( NULL );
		printf( "Número de segundos transcurridos desde el comienzo del programa: %f s\n", difftime(final, comienzo) );
		PcbAux->contadorProgram++;
		int diff =  difftime(final, comienzo);
		int porcentaje= porcentajeDeUso(diff, PcbAux->contadorProgram, retardo);
		printf("El porcentaje de uso es %d\n", porcentaje);



		t_msgHeader header;
		memset(&header, 0, sizeof(t_msgHeader));
		header.msgtype = 4;
		header.payload_size = 0;
		send(planificador, &header, sizeof( t_msgHeader), 0);

		PCB_PARCIAL parcial;
		memset(&parcial, 0, sizeof(PCB_PARCIAL));
		parcial.contadorDePrograma= PcbAux->contadorProgram;
		parcial.pid= PcbAux->PID;
		parcial.tiempo= tiempo;
		send(planificador, &parcial, sizeof( PCB_PARCIAL), 0);

		printf("mProc %s en entrada-salida de tiempo %d\n", PcbAux->nombreProc,tiempo);
		printf("El contador de programa es:%d\n", PcbAux->contadorProgram);
		//log_info(logger, "El pid es %d", PcbAux->PID);
		//log_info(logger, "mProc %s en entrada-salida de tiempo %d",PcbAux->nombreProc, tiempo);



		//actualizamos el porcentaje de cpu en la cola
		usoCPU* nodo =malloc(sizeof(usoCPU*));
		nodo =buscarNodo(PcbAux->cpu_asignada, porcentajes_CPU);
		nodo->porcentaje=porcentaje;
		pthread_mutex_lock(&mutex);
		queue_push(porcentajes_CPU, nodo);
		pthread_mutex_unlock(&mutex);

		//-------

		punta=1501;
		break;
	}
	case 4://Finalizar
	{
		instruccion =4 ;
		printf("Encontre Finalizar\n");
		punta+=200;
		int paginas=0;

		enviarSolicitud (PcbAux->PID, instruccion, paginas , memoria);
		char msj = *(recibirMsjMemoria(memoria));
		printf("mensaje de la memoria %d \n",msj);

		if(msj==1)
		{
			printf("El proceso Pudo finalizar\n");
			log_info(logger, "El pid es %d", PcbAux->PID);
			log_info(logger, "mProc %s finalizado", PcbAux->nombreProc);
			PcbAux->contadorProgram++;


			printf("El contador de programa es:%d\n", PcbAux->contadorProgram);


			final = time( NULL );
			//printf( "Número de segundos transcurridos desde el comienzo del programa: %f s\n", difftime(final, comienzo) );
			int diff =  difftime(final, comienzo);
			int porcentaje= porcentajeDeUso(diff, PcbAux->contadorProgram, retardo);
			printf("El porcentaje de uso es %d\n", porcentaje);

			t_msgHeader header;
			memset(&header, 0, sizeof(t_msgHeader));
			header.msgtype = 3;
			header.payload_size = 0;
			send(planificador, &header, sizeof( t_msgHeader), 0);

			PCB_PARCIAL parcial;
			memset(&parcial, 0, sizeof(PCB_PARCIAL));
			parcial.pid = PcbAux->PID;
			parcial.tiempo = 0;
			parcial.contadorDePrograma = PcbAux->contadorProgram;
			send(planificador, &parcial, sizeof( PCB_PARCIAL), 0);

			//actualizamos el porcentaje de cpu en la cola
			/*usoCPU* nodo =malloc(sizeof(usoCPU*));
			nodo =buscarNodo(PcbAux->cpu_asignada, porcentajes_CPU);
			nodo->porcentaje=porcentaje;
			pthread_mutex_lock(&mutex);
			queue_push(porcentajes_CPU, nodo);
			pthread_mutex_unlock(&mutex);
			free(nodo);*/
			//-------


			usleep((retardo*1000000));
		}
		break;
	}
	case -1://Error
	{
		printf("Error de sentencias!!!");
		punta=1501;
		break;
	}
	}
	return punta;
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


int ubicarPunta(char cadena [1500], PCB* PcbAux){
	int punta=0;
	int i =0;
	int tamBuff=strlen(cadena);
	int contador = PcbAux->contadorProgram;
	while (punta<(tamBuff-1) && contador>i){

		if(cadena[punta]==';'){
			i++;
		}
		punta++;
	}
	if (contador !=0){
		punta++;
	}
	return punta;
}


void sentenciaFinalizar(int memoria, int planificador,t_log* logger, PCB* PcbAux, int retardo){
	int instruccion = 4;
	int paginas=0;
	enviarSolicitud (PcbAux->PID, instruccion, paginas , memoria);

	printf("El proceso Pudo finalizar\n");
	log_info(logger, "El pid es %d", PcbAux->PID);
	log_info(logger, "mProc %s solo ejecuta sentencia finalizado", PcbAux->nombreProc);

	t_msgHeader header;
	memset(&header, 0, sizeof(t_msgHeader));
	header.msgtype = 3;
	header.payload_size = 0;
	send(planificador, &header, sizeof( t_msgHeader), 0);

	PCB_PARCIAL parcial;
	memset(&parcial, 0, sizeof(PCB_PARCIAL));
	parcial.pid = PcbAux->PID;
	parcial.tiempo = 0;
	parcial.contadorDePrograma = PcbAux->contadorProgram;
	send(planificador, &parcial, sizeof( PCB_PARCIAL), 0);

	usleep((retardo*1000000));
	return;
}

void procesarCadenaConQuantum(int quantum , char cadena[1500], int memoria, int planificador,t_log* logger, PCB* PcbAux, int retardo){

	int punta=ubicarPunta(cadena, PcbAux);
	char comando[15]="";
	char pagina[3];
	char texto[20];
	int i=0;
	time_t comienzo, final;
	comienzo = time( NULL );
	int flag=0;


	if (punta != 0){
		t_msgHeader header;
		memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
		header.msgtype = 2;
		header.payload_size = PcbAux->PID; //en este caso el playload lo usamos para pid
		send(planificador, &header, sizeof( t_msgHeader), 0);


	}

	while(flag!=-1 && (strcmp(comando, "finalizar")) && quantum>i )
	{

		int aux;
		aux= recolectar_instruccion(cadena,comando, punta);
		punta=aux;

		aux =procesar_instruccion(cadena, comando, punta, pagina, memoria, planificador,  logger,PcbAux, retardo, texto,comienzo);
		punta=aux;
		if(punta>1500){
			flag=-1;
		}

		i++;
	}

	final = time( NULL );
	//printf( "Número de segundos transcurridos desde el comienzo del programa: %f s\n", difftime(final, comienzo) );
	int diff =  difftime(final, comienzo);
	int porcentaje= porcentajeDeUso(diff, quantum, retardo);
	//printf("El porcentaje de uso es %d\n", porcentaje);
/*
	//actualizamos el porcentaje de cpu en la cola
	usoCPU* nodo =malloc(sizeof(usoCPU*));
	nodo =buscarNodo(PcbAux->cpu_asignada, porcentajes_CPU);
	nodo->porcentaje=porcentaje;
	pthread_mutex_lock(&mutex);
	queue_push(porcentajes_CPU, nodo);
	pthread_mutex_unlock(&mutex);
	free(nodo);
	//-------
*/
	//-- si el quamtum termino en otro sentencia le avisamos a PLANIFICADOR que termino por quantum
	if(strcmp(comando, "finalizar") && strcmp(comando, "entrada-salida")&& strcmp(comando, "fallo")){
		t_msgHeader header;
		memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
		header.msgtype = 5;
		header.payload_size = PcbAux->PID; //en este caso el playload lo usamos para pid
		send(planificador, &header, sizeof( t_msgHeader), 0);

		PCB_PARCIAL parcial;
		memset(&parcial, 0, sizeof(PCB_PARCIAL)); // Ahora el struct tiene cero en todos sus miembros
		parcial.pid = PcbAux->PID;
		parcial.tiempo = 0;
		parcial.contadorDePrograma = PcbAux->contadorProgram;
		send(planificador, &parcial, sizeof( PCB_PARCIAL), 0);
	}

	log_info(logger, "El pid es %d", PcbAux->PID);
	log_info(logger, "mProc %s finalizo el quantum de %d", PcbAux->nombreProc, quantum);
	return;
}


void procesarCadena(char cadena[1500], int memoria, int planificador,t_log* logger, PCB* PcbAux, int retardo){
	//--inicializamos variables y ubicamos la punta pra comenzar a leer (contador de prog)
	int punta=ubicarPunta(cadena, PcbAux);
	int tamBuff=strlen(cadena);
	char comando[15];
	char pagina[3];
	char texto[20];

	time_t comienzo;
	comienzo = time( NULL );

	if (punta != 0){
		t_msgHeader header;
		memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
		header.msgtype = 2;
		header.payload_size = PcbAux->PID; //en este caso el playload lo usamos para pid
		send(planificador, &header, sizeof( t_msgHeader), 0);


	}

	//--mientras no punta sea menor al tamaño del mProc
	while(punta<(tamBuff-1))
	{
		int aux;
		aux= recolectar_instruccion(cadena,comando, punta);
		punta=aux;

		aux =procesar_instruccion(cadena, comando, punta, pagina, memoria, planificador,  logger,PcbAux, retardo, texto, comienzo);
		punta=aux;
	}
	return;
}


void abrir(PCB* PcbAux, char buffer[1500])
{
	//--abrimos el archivo mCod
	FILE * fp;
	fp = fopen(PcbAux->path, "r");
	int len;
	if( fp == NULL )
	{
		perror ("Error al abrir el archivo");
	}

	//--nos colocamos al final del archivo y calculamos el tamaño
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);

	//--nos colocamos de nuevo al principio para pasar los datos al buffet
	fseek(fp, 0, SEEK_SET);
	if (len != fread(buffer, sizeof(char), len+10, fp))
	{
		free(buffer);
	}
	fclose(fp);
}

void* conectar(void* mensa){
	//--inicializamos estrucctura para recibir parametros
	struct1 *param;
	//param=malloc(sizeof(struct1));
	param=(struct1*)mensa;

	//las asignamos a variables locales
	int puertoPlanificador = param->puerto_escucha_planificador;
	char* ipPlanificador = param ->ip_conec_plani;
	int puertoMemoria = param-> puerto_escucha_memoria;
	char* ipMemoria = param->ip_conec_memoria;
	t_log* logger = param->logger;
	int retardo = param->retardo;

	//printf("El ID de hilito es:%u\n", (unsigned int)pthread_self());

	//--nos conectamos con planificador y memoria
	int planificador = conectar_cliente(puertoPlanificador, ipPlanificador);
	int memoria = conectar_cliente(puertoMemoria, ipMemoria);

	//--verificamos que nos hayamos conectado correctamente a memoria, loggueamos
	if (memoria > 0) {
		//log_info(logger, "El hilo %u se conecto a memoria correctamente", (unsigned int) pthread_self());
	} else {
		log_info(logger, "error al conectarse con memoria");
	}

	//--creamos un nodo para esta cpu especifica con el porcentaje de uso
	usoCPU *new = malloc( sizeof(usoCPU) );
	new->cpu=planificador;
	new->porcentaje=0;
	pthread_mutex_lock(&mutex);
	queue_push(porcentajes_CPU,new);//Voy metiendo los porcentajes en la cola
	pthread_mutex_unlock(&mutex);

	printf("El planificador es %d\n", planificador);

	char* aux = recibirMensaje(planificador);
	free(aux);

	while(1){
		//avisamos al planificador que estamos listo para recibir un mProc
		//primera estructura
		t_msgHeader header2;
		memset(&header2, 0, sizeof(t_msgHeader));
		header2.msgtype = 0; //significa estoy libre
		header2.payload_size = planificador;
		send(planificador, &header2, sizeof( t_msgHeader), 0);

		//inicializamos variables para calcular el porcentaje de uso
		time_t comienzo, final;
		comienzo = time( NULL );


		//primero recibimos el tamaño del msj del planificador
		t_msgHeader header;
		memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
		recv(planificador, &header, sizeof( t_msgHeader), 0);

		//recibimos el msj del planificador pcb
		char* buffer;
		PCB *PcbAux =malloc(sizeof(PCB));
		PcbAux->PID=0;
		PcbAux->contadorProgram=0;
		PcbAux->cpu_asignada=0;
		PcbAux->nombreProc=malloc(50);
		PcbAux->path=malloc(200);
		PcbAux->quantum=0;

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

		//--calculamos cuanto tardo el planificador en mandarnos un mProc
		final = time( NULL );
		int diff =  difftime(final, comienzo);

		//--si tardo mas de 60s el porcentaje de uso es 0%
		if(diff >= 60){
			//actualizamos el porcentaje de cpu en la cola
			usoCPU* nodo =malloc(sizeof(usoCPU*));
			nodo =buscarNodo(PcbAux->cpu_asignada, porcentajes_CPU);
			nodo->porcentaje=0;
			pthread_mutex_lock(&mutex);
			queue_push(porcentajes_CPU, nodo);
			pthread_mutex_unlock(&mutex);
			free(nodo);
			//-------
		}
		/*else{
			int instruccion =1;
			int porcentaje = porcentajeDeUso(diff, instruccion,1);
			//actualizamos el porcentaje de cpu en la cola
			usoCPU* nodo =malloc(sizeof(usoCPU*));
			nodo =buscarNodo(PcbAux->cpu_asignada, porcentajes_CPU);
			nodo->porcentaje=porcentaje;
			pthread_mutex_lock(&mutex);
			queue_push(porcentajes_CPU, nodo);
			pthread_mutex_unlock(&mutex);
			free(nodo);
			//-------

		}*/

		printf("El mensaje del planificador PCB es:\n");
		printf("Este es el PID:                  %d\n", PcbAux->PID);
		printf("Este es el contador de programa: %d\n",PcbAux->contadorProgram);
		printf("La CPU asignada es:              %d\n",PcbAux->cpu_asignada);
		printf("El nombre  del proceso es:       %s\n", PcbAux->nombreProc);
		printf("El path es:                      %s\n", PcbAux->path);
		printf("El quantum es:                   %d\n", PcbAux->quantum);
/*
		log_info(logger, "----------------PCB-------------------------------------------------------------------------------------------------------------------------------------------------------------");
		log_info(logger, "Este es el PID:                  %d\n", PcbAux->PID);
		log_info(logger, "Este es el contador de programa: %d\n",PcbAux->contadorProgram);
		log_info(logger, "La CPU asignada es:              %d\n",PcbAux->cpu_asignada);
		log_info(logger, "El nombre  del proceso es:       %s\n", PcbAux->nombreProc);
		log_info(logger, "El path es:                      %s\n", PcbAux->path);
		log_info(logger, "El quantum es:                   %d\n", PcbAux->quantum);
		log_info(logger, "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
*/

		//contadorProgram; si es -1 no abrimos, ejecutamos solo FINALIZAR
		if(PcbAux->contadorProgram!= -1){
			//abrimos el mProc y lo guardamos en un buffet para leerlo
			char archivoProc[1500];
			memset(archivoProc,'\0', 1500);
			abrir(PcbAux,archivoProc);

			// si el quantum es 0 la planificacion es FIFO, sino es ROUND ROBIN
			if(PcbAux->quantum == 0){
				//fifo
				printf("El quantum es %d\n", PcbAux->quantum);
				procesarCadena(archivoProc, memoria, planificador,logger, PcbAux, retardo);

			}else{
				printf("El quantum es %d\n", PcbAux->quantum);
				procesarCadenaConQuantum(PcbAux->quantum, archivoProc, memoria, planificador, logger, PcbAux, retardo);
			}
		}else{
			sentenciaFinalizar(memoria, planificador, logger, PcbAux, retardo);
		}

		free(buffer);
	}

	free(new);
	//free(param);
	return EXIT_SUCCESS;
}












