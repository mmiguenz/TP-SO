/*
 * particionSwap.c

 *
 *  Created on: 11/10/2015
 *      Author: utnso
 */
#include  <stdio.h>
#include "particionSwap.h"
#include "swapConfig.h"
#include <commons/string.h>

int paginaActual(t_particion* particion);
void posicionarProximaPagina(t_particion*);
void posicionarPagina(t_particion*, int );
int hayEspacio(t_particion* particion, int espacio);
int hayEspacioContiguo(t_particion* particion,int espacioRequerido);
void compactar(t_particion* particion);
void reAsignarHuecos( t_particion* particion, int paginaComienzo, int cantidadPaginas);

t_particion* t_particion_crear(t_swapConfig* config)
{
	t_particion* particion  = malloc(sizeof(t_particion));
	char* script= malloc(100);
	char* cantidad_Paginas= malloc(4);
	char* tamanio_pagina = malloc(4);


	cantidad_Paginas = string_itoa(config->cantidad_Paginas);
	tamanio_pagina = string_itoa(config->tamanio_Pagina);


	strcpy(script,"dd if=/dev/zero of=");
	strcpy(script,config->nombre_Swap);
	strcat(script," bs=");
	strcat(script,tamanio_pagina);
	strcat(script," count=");
	strcat(script,cantidad_Paginas);

	system(script);

	particion->archivoParticion = fopen("particion","r+");
	particion->archivo_tamanio = config->cantidad_Paginas * config->tamanio_Pagina ;
	particion->pagina_tamanio= config->tamanio_Pagina ;
	particion->paginas_cantidad= config ->cantidad_Paginas;


	particion->espacioLibre = list_create();


	t_hueco* unHueco = t_hueco_crear(0,config->cantidad_Paginas);
	list_add(particion->espacioLibre,(void*)unHueco);


     return particion;


}

void t_particion_elminar(t_particion* particion)
{
	fclose(particion->archivoParticion);
	free(particion);


}

void* t_particion_leerPagina(t_particion* particion ,int numeroDePagina)
{
	posicionarPagina(particion,numeroDePagina);
	void* pagina = malloc(particion->pagina_tamanio);

	fread(pagina,particion->pagina_tamanio,1,particion->archivoParticion);

	return pagina;

}

int paginaActual(t_particion* particion)
{

	int posicion = ftell(particion->archivoParticion);

	return (int) (posicion / (particion->pagina_tamanio));

}

void posicionarProximaPagina(t_particion* particion)
{
	fseek(particion->archivoParticion,particion->pagina_tamanio,SEEK_CUR);


}


void posicionarPagina(t_particion* particion, int numeroDePagina)
{
	int p_Actual = paginaActual(particion);

	int ultimaPagina = particion->paginas_cantidad;

	if(p_Actual > numeroDePagina)
		{
			rewind(particion->archivoParticion);

		}


	while(p_Actual< ultimaPagina && p_Actual!= numeroDePagina )
	{
		if(p_Actual<numeroDePagina)
		{
			posicionarProximaPagina(particion);
			p_Actual = paginaActual(particion);


		}


	}
}


int t_particion_reservarPaginas(t_particion* particion, int cantidadDePaginas)
{

	int paginaComienzo  =hayEspacioContiguo(particion,cantidadDePaginas);

	if(paginaComienzo ==-1)
	{
		compactar(particion);
		return t_particion_reservarPaginas(particion,cantidadDePaginas);

	}else
	{
		reAsignarHuecos(particion,paginaComienzo,cantidadDePaginas);
		return paginaComienzo;

	}



}




// si hay espacio contiguo, retorna el nro de pagina de comienzo
//caso contrario, -1 .
int hayEspacioContiguo(t_particion* particion,int espacioRequerido)
{
	bool criterioBusHueco(t_hueco* hueco)
	{
		return hueco->cantidadPaginas>=espacioRequerido;

	}


	t_hueco* huecoEncontrado=   list_find(particion->espacioLibre,(void*)criterioBusHueco);

	return (huecoEncontrado!=NULL)?huecoEncontrado->paginaInicio:-1;

}


void compactar(t_particion* particion)
{


}



t_hueco* t_hueco_crear(int inicio, int cantidad)
{
	t_hueco* unHueco = malloc(sizeof(t_hueco));
	unHueco->cantidadPaginas = cantidad;
	unHueco->paginaInicio = inicio;


	return unHueco;


}
void t_hueco_eliminar(t_hueco* unHueco)
{
	free(unHueco);


}


void reAsignarHuecos( t_particion* particion, int paginaComienzo, int cantidadPaginas)
{



}

