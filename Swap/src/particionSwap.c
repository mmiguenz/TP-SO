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

t_particion* t_particion_crear(t_swapConfig* config)
{
	t_particion* particion  = malloc(sizeof(t_particion));
	char* script= malloc(100);
	char* cantidad_Paginas= malloc(4);
	char* tamanio_pagina = malloc(4);


	cantidad_Paginas = string_itoa(config->cantidad_Paginas);
	tamanio_pagina = string_itoa(config->tamanio_Pagina);


	strcpy(script,"dd if=/dev/zero of=particion");
	strcat(script," bs=");
	strcat(script,tamanio_pagina);
	strcat(script," count=");
	strcat(script,cantidad_Paginas);

	system(script);

	particion->archivoParticion = fopen("particion","r+");
	particion->archivo_tamanio = config->cantidad_Paginas * config->tamanio_Pagina ;
	particion->pagina_tamanio= config->tamanio_Pagina ;
	particion->paginas_cantidad= config ->cantidad_Paginas;


	particion->bitMapPaginasOcupadas = calloc(particion->paginas_cantidad,sizeof(char));
	int i ;
	for(i=0; i<particion->paginas_cantidad; i++)
		particion->bitMapPaginasOcupadas[i]= 0;


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
	if(!hayEspacio(particion,cantidadDePaginas))
		return -1;

	int paginaComienzo  =hayEspacioContiguo(particion,cantidadDePaginas);

	if(paginaComienzo ==-1)
	{
		compactar(particion);
		return t_particion_reservarPaginas(particion,cantidadDePaginas);

	}else
	{
		int i ;
		for(i=paginaComienzo; i<cantidadDePaginas;i++)
			(*particion->bitMapPaginasOcupadas[i]) = 1;

		return paginaComienzo;

	}



}

int hayEspacio(t_particion* particion,int espacioRequerido)
{
	int espacioVacioEncontrado=0;

	int i ;
	for(i=0; i< particion->paginas_cantidad;i++)
	{
		if(*(particion->bitMapPaginasOcupadas[i])== 0 )
			espacioVacioEncontrado++;

	}

	return espacioVacioEncontrado>=espacioRequerido ;


}



// si hay espacio contiguo, retorna el nro de pagina de comienzo
//caso contrario, -1 .
int hayEspacioContiguo(t_particion* particion,int espacioRequerido)
{
	int espacioVacioEncontrado=0;

	int i ;
	for(i=0; i< particion->paginas_cantidad;i++)
	{
		if(*(particion->bitMapPaginasOcupadas[i])== 0 )
		{

			if(++espacioVacioEncontrado== espacioRequerido)
				return  (i - espacioRequerido);


		}else
		{

			espacioVacioEncontrado= 0;

		}



	}

	return -1 ;


}


void compactar(t_particion* particion)
{


}
