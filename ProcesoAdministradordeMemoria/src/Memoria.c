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

t_dictionary* dicInfoProcesos;
extern  char** memoria;


char ** inicializarMemoriaPrincipal(int Cant_Marcos,int Tamanio_Marco)
{
	int i;
	char** memoriaPrincipal;
	memoriaPrincipal = malloc(sizeof(char*) * Cant_Marcos);

		//Creo los MARCOS
		for (i = 0; i < Cant_Marcos; i++)
		{
					char* tmp = malloc(sizeof(char) * Tamanio_Marco);
					int tamanioDeTextoParaIniciar = strlen("Valor por defecto :D");
					memcpy(tmp, "Valor por defecto :D", tamanioDeTextoParaIniciar);
					tmp[tamanioDeTextoParaIniciar] = '\0';
					memoriaPrincipal[i] = tmp;
		}

	return memoriaPrincipal;
}
int* inicializarMemoriaLibre(int cant_Marcos)
{
	int* memoriaLibre;
	/*
		Array de marcos libres:
			Posicion = 0 -> LIBRE  |  Posicion = 1 -> OCUPADA
	*/
	memoriaLibre = calloc(cant_Marcos, sizeof(char));


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


t_list* asignarMarcos_MemoriaPrincipal(t_list* marcosAsignados, int* memoriaLibre, int MarcosPorProceso)
		{
			int i;
			int* marcoAsignado = malloc(sizeof(int));
			for(i = 0; i < MarcosPorProceso; i++)
			{
				*marcoAsignado = algoritmo_FirstFit_MEMORIA(MarcosPorProceso,memoriaLibre);
				memoriaLibre[*marcoAsignado] = 1;
				list_add(marcosAsignados, marcoAsignado);
			}

			return marcosAsignados;
		}

int algoritmo_FirstFit_MEMORIA(int Cant_Marcos, int* memoriaLibre)
{
	int posicionActual;

	for (posicionActual = 0; posicionActual < Cant_Marcos; posicionActual++)
	{
		if(memoriaLibre[posicionActual] == 0)
			return posicionActual;
	}

	return -1;
}

void finalizarProceso(int pid, MEMORIAPRINCIPAL* memoriaP)
{
 char ** memoria = 	memoriaP->Memoria;

			TABLADEPROCESOS* procesoAux = dictionary_remove(dicInfoProcesos, pid);
			int i;
			int totalDePaginas = (*procesoAux).totalDePaginas;
			int** tablaDePaginas = (*procesoAux).tablaDePaginas;
			t_list* marcosAsignados = (*procesoAux).marcosAsignados;
			t_queue * contenidoDeMarcos = (*procesoAux).contenidoDeMarcos;

			//elimina tabla de paginas del proceso
				for(i=0; i<totalDePaginas; i++)
				free(tablaDePaginas[i]);
				free(tablaDePaginas);

			//libero marcos de memoria Principal
			list_iterate(marcosAsignados, resetearMarco);//ver parametros
			list_clean(marcosAsignados);
			list_destroy(marcosAsignados);
			queue_clean(contenidoDeMarcos);
			queue_destroy(contenidoDeMarcos);
			free(procesoAux);

	return;
}


void resetearMarco(void * numeroMarco)
{
	int * numMarco = (int*)numeroMarco;
	//memcpy(memoria[numeroMarco],"Marco Libre" , strlen("Marco Libre"));
	//memoria[numeroMarco][strlen("Marco Libre")] = '\0';
	return;
}



void inicializarProceso(int PID, int totalDePaginas, int* memoriaLibre, int marcosPorProcesos)
{
TABLADEPROCESOS* ProcesoX = malloc(sizeof(TABLADEPROCESOS*));

(*ProcesoX).totalDePaginas = totalDePaginas;

(*ProcesoX).tablaDePaginas = inicializarTablaDePaginas(totalDePaginas);

t_list* marcosAsignados = list_create();

marcosAsignados = asignarMarcos_MemoriaPrincipal(marcosAsignados, memoriaLibre,marcosPorProcesos);

(*ProcesoX).marcosAsignados = marcosAsignados;

(*ProcesoX).contenidoDeMarcos = queue_create();

(*ProcesoX).contadorMissTLB = 0;
(*ProcesoX).contadorHits = 0;


return;
}


int ** inicializarTablaDePaginas(int cantPagina)
{

	int i;

	    int** tablaParaPagina = malloc(cantPagina*sizeof(int*));
	    for (i=0; i<cantPagina; ++i)
	    {
	    	tablaParaPagina[i] = calloc(3, sizeof(int));
	    }

	return tablaParaPagina;
}
