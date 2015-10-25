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


int buscarFrameLibre(MEMORIAPRINCIPAL* memoria);



void t_memoria_crear(MEMORIAPRINCIPAL* memoriaP , t_paramConfigAdmMem* config)
{


	memoriaP->Memoria=inicializarMemoriaPrincipal(memoriaP,config->cantidad_marcos,config->tamanio_marco);
	memoriaP->MemoriaLibre=inicializarMemoriaLibre(config->cantidad_marcos);
	memoriaP->cantMarcos = config->cantidad_marcos;
	memoriaP->tamanioMarco = config->tamanio_marco;




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
						int tamanioDeTextoParaIniciar = strlen("Valor por defecto :D");
						memcpy(tmp, "Valor por defecto :D", tamanioDeTextoParaIniciar);
						tmp[tamanioDeTextoParaIniciar] = '\0';
						marco= tmp;



}

char** inicializarMemoriaLibre(int cant_Marcos)
{
	char** memoriaLibre;
	int i;
	/*
		Array de marcos libres:
			Posicion = 0 -> LIBRE  |  Posicion = 1 -> OCUPADA
	*/
	memoriaLibre = calloc(cant_Marcos, sizeof(char));
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

	for (i = 0 ; i < tablaDePaginas->cantTotalPaginas; i++)
	{
		t_regPagina* pagina = tablaDePaginas->Pagina[i];

		 inicializarMarco(memoriaP, memoriaP->Memoria[pagina->idFrame]);
		 *(memoriaP->MemoriaLibre[pagina->idFrame]) = 0;


	}


	return;
}


int buscarPaginaenMemoria(int pid, int pagina,t_dictionary* tablasPagsProcesos){
	char* pidBuscado = string_itoa(pid);
	int frame = -1;
	t_tablaDePaginas* tablaPaginasProceso = malloc(sizeof(t_regPagina**)+sizeof(int));

	if (!(dictionary_is_empty(tablasPagsProcesos) && dictionary_has_key(tablasPagsProcesos,pidBuscado))){
		tablaPaginasProceso = dictionary_get(tablasPagsProcesos,pidBuscado);
		if (tablaPaginasProceso->Pagina[pagina]->bitPresencia){
			frame = tablaPaginasProceso->Pagina[pagina]->idFrame;
		}
	}
	return frame;
}

int insertarContenidoenMP(int socketSwap,char*contenido,MEMORIAPRINCIPAL memoria, t_tablaDePaginas* tablaPagsProceso){
int i;
int marco;

/*reveer */
	while (memoria.MemoriaLibre[i] == 0 && i< memoria.cantMarcos){
		i++;
	}
	if (memoria.MemoriaLibre[i] != 0){
		memoria.Memoria[i] = contenido;
		marco = i;
	}
	else {//posible switch a implementar con el algoritmo de reemplazo indicado por arch de config.
	marco = reemplazarPaginaFIFO(socketSwap,contenido,memoria,tablaPagsProceso);
	memoria.Memoria[marco] = contenido;
	}
	return marco;
}

int reemplazarPaginaFIFO (int socketSwap,char* contenido, MEMORIAPRINCIPAL memoria, t_tablaDePaginas* tablaPagsProceso){
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
	int tamanioContenido;
	if(PaginaFirstIn->bitModificado){
		tamanioContenido = strlen(memoria.Memoria[PaginaFirstIn->idFrame]);
		char* contenidoReemp = malloc((sizeof(char)*(tamanioContenido+1)));
		contenidoReemp = memoria.Memoria[PaginaFirstIn->idFrame];
		enviarDatosPorModifASwap(socketSwap,contenidoReemp,pagAModif,tablaPagsProceso->pid);
		frame = PaginaFirstIn->idFrame;
	}
	PaginaFirstIn->bitPresencia = 0;
	PaginaFirstIn->bitModificado = 0;
	PaginaFirstIn->horaIngreso = 90000000;
	PaginaFirstIn->idFrame = -1;
	return frame;

}

void enviarDatosPorModifASwap(int swapSocket,char* contenidoReemp,int pagAModif,int pid){
	t_protoc_escrituraProceso* protocEscrSwap = malloc(sizeof(t_protoc_escrituraProceso));
	int tamanioContenido = strlen(contenidoReemp)+1;
	int offset;
	int tamanioBuffer = (sizeof(int)*3)+(sizeof(char)*tamanioContenido)+1;
	void* buffer = malloc(tamanioBuffer);

	protocEscrSwap->tipoInstrucc = 2;
	protocEscrSwap->pid = pid;
	protocEscrSwap->pagina = pagAModif;
	protocEscrSwap->contenido = contenidoReemp;

	offset = sizeof(char);
	memcpy(buffer,&(protocEscrSwap->tipoInstrucc),sizeof(char));
	memcpy(buffer+offset,&(protocEscrSwap->pid),sizeof(int));
	offset += sizeof(int);
	memcpy(buffer+offset,&(protocEscrSwap->pagina),sizeof(int));
	offset += sizeof(int);
	memcpy(buffer+offset,&tamanioContenido,sizeof(int));
	offset += sizeof(int);
	memcpy(buffer+offset,protocEscrSwap->contenido,(sizeof(char)*tamanioContenido));

	send(swapSocket,buffer,tamanioBuffer,0);

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

void actualizarTablaPagsProceso(int frame,int pagina,t_tablaDePaginas* tablaPagsProceso){
	tablaPagsProceso->Pagina[pagina]->bitPresencia = 1;
	tablaPagsProceso->Pagina[pagina]->bitModificado=1;
	tablaPagsProceso->Pagina[pagina]->horaIngreso = convertirTimeStringToInt(temporal_get_string_time());
	tablaPagsProceso->Pagina[pagina]->idFrame = frame;
}


int t_cargarContenido(MEMORIAPRINCIPAL* memoriaP,char* contenido)
{
	int frame = buscarFrameLibre(memoriaP);

	memoriaP->Memoria[frame]  = contenido;
	*(memoriaP->MemoriaLibre[frame]) = 1 ;
	return frame ;






}

int buscarFrameLibre(MEMORIAPRINCIPAL* memoria)
{
	int i;
	for (i = 0; memoria->cantMarcos;i++)
	{
		if(memoria->MemoriaLibre[i]==0)
			return i;
	}

	return -1;


}


int t_hayFrameLibre(MEMORIAPRINCIPAL* memoria)
{
	return buscarFrameLibre(memoria)>=0? 1 : 0;

}

