/*
 * EstructurasParaMemoria.c
 *
 *  Created on: 6/10/2015
 *      Author: utnso
 */
#include "Memoria.h"

#include <commons/collections/dictionary.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>
#include <commons/temporal.h>
#include <sys/socket.h>
#include "AdministradordeMemoria.h"
#include "ProtocsyFuncsRecvMsjs.h"

#define MAXTIME 90000000;


void t_memoria_crear(MEMORIAPRINCIPAL* memoriaP , t_paramConfigAdmMem* config)
{

	memoriaP->cantMarcos = config->cantidad_marcos;
	memoriaP->tamanioMarco = config->tamanio_marco;
	memoriaP->Memoria=inicializarMemoriaPrincipal(memoriaP,config->cantidad_marcos,config->tamanio_marco);
	memoriaP->MemoriaLibre=inicializarMemoriaLibre(config->cantidad_marcos);

}


char ** inicializarMemoriaPrincipal(MEMORIAPRINCIPAL* memoriaP ,int Cant_Marcos,int Tamanio_Marco)
{
	int i;
	char** memoriaPrincipal;

	memoriaPrincipal = malloc(sizeof(char*) * Cant_Marcos);



		//Creo los MARCOS
		for (i = 0; i < Cant_Marcos; i++)
		{

				inicializarMarco(memoriaP,memoriaPrincipal[i]);
		}

	return memoriaPrincipal;
}


void inicializarMarco(MEMORIAPRINCIPAL* memoriaP, char* marco)
{
	char* tmp = malloc(sizeof(char) * memoriaP->tamanioMarco);
						memset(tmp,'\0',memoriaP->tamanioMarco);
						memcpy(tmp, "a", 2);
						marco =tmp;

}

char* inicializarMemoriaLibre(int cant_Marcos)
{
	char* memoriaLibre;
	int i;
	/*
		Array de marcos libres:
			Posicion = 0 -> LIBRE  |  Posicion = 1 -> OCUPADA
	*/
	memoriaLibre = malloc((cant_Marcos* sizeof(char)));
	for (i=0;i<cant_Marcos;i++){
		memoriaLibre[i] = 0;
	}


	return memoriaLibre;
}

int Marcoslibres(int cantidadDePaginas, int Cant_Marcos, int* memoriaLibre)
{
	int posicionActual;
	int cantidadEspaciosLibres = 0;

	for (posicionActual = 0; posicionActual < Cant_Marcos; posicionActual++)
	{
		if(memoriaLibre[posicionActual] == 0)
			cantidadEspaciosLibres++;
	}

	if (cantidadEspaciosLibres >= cantidadDePaginas)
	{
		return 1;
	}
	return 0;
}


void finalizarProceso(MEMORIAPRINCIPAL* memoriaP ,t_tablaDePaginas* tablaDePaginas)
{
	int i ;

	for (i = 0 ; i < (tablaDePaginas->cantTotalPaginas); i++)
	{
		t_regPagina* pagina = tablaDePaginas->Pagina[i];

		if (tablaDePaginas->Pagina[i]->idFrame != -1){
		 inicializarMarco(memoriaP, memoriaP->Memoria[pagina->idFrame]);
		 memoriaP->MemoriaLibre[pagina->idFrame] = 0;
		}

	}


	return;
}


int buscarPaginaenMemoria(int pid, int pagina,t_dictionary* tablasPagsProcesos){
	char* pidBuscado = string_itoa(pid);
	int frame = -1;
	t_tablaDePaginas* tablaPaginasProceso;

	if (!(dictionary_is_empty(tablasPagsProcesos) && dictionary_has_key(tablasPagsProcesos,pidBuscado))){
		tablaPaginasProceso = dictionary_get(tablasPagsProcesos,pidBuscado);
		if (tablaPaginasProceso->Pagina[pagina]->bitPresencia){
			frame = tablaPaginasProceso->Pagina[pagina]->idFrame;
		}
	}

	return frame;
}

void insertarPaginaenMP(char*contenido,MEMORIAPRINCIPAL* memoria,int* marco){

	memoria->Memoria[*marco] = contenido;
	memoria->MemoriaLibre[*marco] = 1;

}

int reemplazarPaginaFIFO (int socketSwap,char* contenido, MEMORIAPRINCIPAL* memoria, t_tablaDePaginas* tablaPagsProceso,int* pagAReemp){
	int i;
	int pagAModif = 0;
	int frame;
	t_regPagina* PaginaFirstIn;
	PaginaFirstIn = tablaPagsProceso->Pagina[0];
	for (i=1;i<tablaPagsProceso->cantTotalPaginas;i++){

		if (tablaPagsProceso->Pagina[i]->horaIngreso < PaginaFirstIn->horaIngreso){
			PaginaFirstIn = tablaPagsProceso->Pagina[i];
			pagAModif = i;
		}
	}
	PaginaFirstIn->bitPresencia = 0;
	*pagAReemp = pagAModif;

	if(PaginaFirstIn->bitModificado){
		int tamanioContenido = memoria->tamanioMarco;
		char* contenidoReemp = malloc(memoria->tamanioMarco);
		contenidoReemp = memoria->Memoria[PaginaFirstIn->idFrame];
		enviarDatosPorModifASwap(socketSwap,contenidoReemp,tamanioContenido,pagAModif,tablaPagsProceso->pid);
		//free(contenidoReemp);
	}

	frame = PaginaFirstIn->idFrame;
	PaginaFirstIn->bitPresencia = 0;
	PaginaFirstIn->bitModificado = 0;
	PaginaFirstIn->horaIngreso = MAXTIME;
	PaginaFirstIn->idFrame = -1;

	return frame;

}

void enviarDatosPorModifASwap(int swapSocket,char* contenidoReemp, int tamanioContenido,int pagAModif,int pid){
	t_protoc_escrituraProceso* protocEscrSwap = malloc(sizeof(t_protoc_escrituraProceso));
	int offset;
	int tamanioBuffer = (sizeof(int)+sizeof(int)+sizeof(int)+tamanioContenido+1);
	void* buffer = malloc(tamanioBuffer);

	protocEscrSwap->tipoInstrucc = ESCRIBIR;
	protocEscrSwap->pid = pid;
	protocEscrSwap->pagina = pagAModif;
	protocEscrSwap->tamanio = tamanioContenido;
	protocEscrSwap->contenido = malloc(sizeof(tamanioContenido));
	protocEscrSwap->contenido = contenidoReemp;
	int tamanioString = strlen(contenidoReemp)+1;

	offset = sizeof(char);
	memcpy(buffer,&(protocEscrSwap->tipoInstrucc),sizeof(char));
	memcpy(buffer+offset,&(protocEscrSwap->pid),sizeof(int));
	offset += sizeof(int);
	memcpy(buffer+offset,&(protocEscrSwap->pagina),sizeof(int));
	offset += sizeof(int);
	memcpy(buffer+offset,&(protocEscrSwap->tamanio),sizeof(int));
	offset += sizeof(int);
	memcpy(buffer+offset,protocEscrSwap->contenido,tamanioString);

	send(swapSocket,buffer,tamanioBuffer,0);

	char* confirmSwap = malloc(sizeof(char));
	recv(swapSocket,confirmSwap,sizeof(char),0);

	if (*confirmSwap == 1)
	printf("Se ha reemplazado una página en memoria modificada previamente, actualizando su contenido en Swap de forma satisfactoria \n");

	free(protocEscrSwap);
	free(buffer);

}

int convertirTimeStringToInt(char* time){

	char** splittedTime = calloc(4,(sizeof(char)*4));
	int timeConverted,hourToSegs,minToSegs,segs,miliSecs;

	splittedTime = string_split(time,":");
	hourToSegs = (atoi(splittedTime[0]))*3600;
	minToSegs = (atoi(splittedTime[1]))*60;
	segs = atoi(splittedTime[2]);
	miliSecs = atoi(splittedTime[3]);
	timeConverted = (hourToSegs+minToSegs+segs)*1000;
	timeConverted += miliSecs;
	free(splittedTime);

	return timeConverted;
}

void actualizarTablaPaginas(int instruccion,int frame,int pagina,t_tablaDePaginas* tablaPagsProceso){

	tablaPagsProceso->Pagina[pagina]->bitPresencia = 1;
	tablaPagsProceso->Pagina[pagina]->horaIngreso = convertirTimeStringToInt(temporal_get_string_time());
	tablaPagsProceso->Pagina[pagina]->idFrame = frame;
	actualizarUtilizyModifPag(instruccion,tablaPagsProceso,pagina);
}

void actualizarUtilizyModifPag(int instruccion, t_tablaDePaginas* tablaPagsProceso, int pagina){

tablaPagsProceso->Pagina[pagina]->bitUtilizado = 1;
tablaPagsProceso->Pagina[pagina]->horaUtilizacion = convertirTimeStringToInt(temporal_get_string_time());

char* bitModifdePagina = &(tablaPagsProceso->Pagina[pagina]->bitModificado);
*bitModifdePagina = instruccion == ESCRIBIR?(*bitModifdePagina = 1):*bitModifdePagina;
}


int buscarFrameLibre(MEMORIAPRINCIPAL* memoria)
{
	int i;
	for (i = 0;i < memoria->cantMarcos;i++)
	{

		if((memoria->MemoriaLibre[i])==0)
			return i;
	}

	return -1;


}

int marcosUtilizadosProceso(t_tablaDePaginas* tablaPagsProceso){
	int marcosUtilizados = 0;
	int i = 0;
	for (i = 0; i < tablaPagsProceso->cantTotalPaginas; ++i) {
		if(tablaPagsProceso->Pagina[i]->bitPresencia)
		marcosUtilizados++;
	}
	return marcosUtilizados;
}

void mem_Flush(MEMORIAPRINCIPAL* memoria, t_dictionary* tablasDePaginas){

	int i;

	for (i = 0; i < memoria->cantMarcos; ++i) {

		inicializarMarco(memoria,memoria->Memoria[i]);

	}
	memoria->MemoriaLibre = inicializarMemoriaLibre(memoria->cantMarcos);

	void blanquearTablaPagina(char* pid,t_tablaDePaginas* tablaDePaginas){

		for (i=0; i<tablaDePaginas->cantTotalPaginas; ++i){
				 tablaDePaginas->Pagina[i]->idFrame = -1;
				 tablaDePaginas->Pagina[i]->bitPresencia = 0;
				 tablaDePaginas->Pagina[i]->bitModificado = 0;
				 tablaDePaginas->Pagina[i]->horaIngreso = 90000000;//Espacio que ocupa la hora en formato 'hh:mm:ss:mmmm'
			 }

	dictionary_iterator(tablasDePaginas,(void*)blanquearTablaPagina);
	}

return;
}
