/*
 * particionSwap.c

 *
 *  Created on: 11/10/2015
 *      Author: utnso
 */
#include  <stdio.h>
#include "particionSwap.h"
#include <commons/string.h>
#include "protocolos.h"
#include "logueoSwap.h"
#include "swapConfig.h"
#include "Swap.h"



int paginaActual(t_particion* particion);
void posicionarProximaPagina(t_particion*);
void posicionarPagina(t_particion*, int );
int hayEspacioContiguo(t_particion* particion,int espacioRequerido);
void compactar(t_particion* particion,t_list* espacioUtilizado_lista);
void reAsignarHuecosPorInicio( t_particion* particion, int paginaComienzo, int cantidadPaginas);
void reAsignarHuecosPorCompactacion(t_particion* particion,int proximaPaginaLibre);
void unificarHuecos(t_particion* particion);
t_hueco* unificar(t_hueco* unHueco,t_hueco* otroHueco);
int sonContiguos(t_hueco* unHueco,t_hueco* otroHueco);


t_particion* t_particion_crear(t_swapConfig* config,t_log* logSwap)
{


	t_particion* particion  = malloc(sizeof(t_particion));
	char* script= malloc(100);
	char* cantidad_Paginas= malloc(4);
	char* tamanio_pagina = malloc(4);


	cantidad_Paginas = string_itoa(config->cantidad_Paginas);
	tamanio_pagina = string_itoa(config->tamanio_Pagina);


	strcpy(script,"dd if=/dev/zero of=");
	strcat(script,config->nombre_Swap);
	strcat(script," bs=");
	strcat(script,tamanio_pagina);
	strcat(script," count=");
	strcat(script,cantidad_Paginas);

	system(script);


	particion->archivoParticion = fopen(config->nombre_Swap,"r+");
	particion->archivo_tamanio = config->cantidad_Paginas * config->tamanio_Pagina ;
	particion->pagina_tamanio= config->tamanio_Pagina ;
	particion->paginas_cantidad= config ->cantidad_Paginas;
	particion->retardoCompactacion=config->retardo_Compact;
	particion->logSwap = logSwap;

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
	char* contenidoPagina = malloc(particion->pagina_tamanio);
	fread(contenidoPagina,particion->pagina_tamanio,1,particion->archivoParticion);
	return contenidoPagina;
}

void t_particion_escribirPagina(t_particion* particion ,int numeroDePagina, t_protoc_escrituraProceso* pedido)
{

	posicionarPagina(particion,pedido->pagina);
	fwrite(pedido->contenido,pedido->tamanio,1,particion->archivoParticion);
	return;
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
			p_Actual=paginaActual(particion);

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


int t_particion_reservarPaginas(t_particion* particion, int cantidadDePaginas,t_list* espacioUtilizado_lista)
{

	int paginaComienzo  =hayEspacioContiguo(particion,cantidadDePaginas);

	if(paginaComienzo ==-1)
	{
	    t_SwapLog* swapLog  = t_swapLog_crear(0,0,0,NULL);

	    t_loguear(particion->logSwap,COMPATACION,swapLog);
		compactar(particion,espacioUtilizado_lista);
		t_loguear(particion->logSwap,COMPATACION,swapLog);

		t_swapLog_eliminar(swapLog);
		return t_particion_reservarPaginas(particion,cantidadDePaginas,espacioUtilizado_lista);

	}else
	{
		reAsignarHuecosPorInicio(particion,paginaComienzo,cantidadDePaginas);
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


void compactar(t_particion* particion,t_list* espacioUtilizado_lista)
{
	usleep(particion->retardoCompactacion);

	int pagina= 0 ;
	int cantidadDeProcesos =  list_size( espacioUtilizado_lista);
	int i ;

	t_proceso* proceso;
	for(i=0; i< cantidadDeProcesos; i++)
	{
		proceso  = list_get(espacioUtilizado_lista,i);
		int tamaniobuff = (particion->pagina_tamanio) * (proceso->cantidad);
		void* buffer = malloc(tamaniobuff);

		posicionarPagina(particion,proceso->paginaComienzo);
		fread( buffer,tamaniobuff,1,particion->archivoParticion);

		posicionarPagina(particion,pagina);
		fwrite( buffer,tamaniobuff,1,particion->archivoParticion);

		proceso->paginaComienzo = pagina;
		pagina+= proceso->cantidad;


	}

	int proximaPaginaLibre = pagina ;

	reAsignarHuecosPorCompactacion(particion,proximaPaginaLibre);


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


void reAsignarHuecosPorInicio( t_particion* particion, int paginaComienzo, int cantidadPaginas)
{
	bool buscarHuecoPorPagina(t_hueco* unHueco)
	{
		return unHueco->paginaInicio==paginaComienzo;

	}


	t_hueco* hueco = list_find(particion->espacioLibre,(void*)buscarHuecoPorPagina);

	hueco->paginaInicio = paginaComienzo + cantidadPaginas;
	hueco->cantidadPaginas-=cantidadPaginas;

	if (hueco->cantidadPaginas ==0)
	{

		list_remove_and_destroy_by_condition(particion->espacioLibre,(void*)buscarHuecoPorPagina,(void*)t_hueco_eliminar);


	}

}


void reAsignarHuecosPorCompactacion( t_particion* particion,int proximaPaginaLibre)
{
	list_clean_and_destroy_elements(particion->espacioLibre, (void*)t_hueco_eliminar);



	t_hueco* unHueco = t_hueco_crear(proximaPaginaLibre,(particion->paginas_cantidad - proximaPaginaLibre));

	list_add(particion->espacioLibre ,(void*) unHueco);

	t_particion_limpiar(particion,unHueco->paginaInicio,unHueco->cantidadPaginas);


}

void t_hueco_agregar(t_particion* particion,int paginaComienzo,int cantidad)
{
	t_hueco* unHueco = t_hueco_crear(paginaComienzo,cantidad);

	list_add(particion->espacioLibre,(void*)unHueco);

	unificarHuecos(particion);


}



void unificarHuecos(t_particion* particion)
{
	bool ordenaDeMenorAMayorPorPaginaDeInicio(t_hueco* unHueco,t_hueco* otroHueco ){

		return unHueco->paginaInicio<otroHueco->paginaInicio;

	}


	t_hueco *unHueco, *otroHueco;
	int cantidadHuecos;
	t_list* huecos = particion->espacioLibre;
	list_sort(huecos,(void*)ordenaDeMenorAMayorPorPaginaDeInicio);


	cantidadHuecos = list_size(huecos);
	int i;
	unHueco= list_get(huecos,0);
	for(i=1; i<cantidadHuecos; i++)
	{
		otroHueco = list_get(huecos,(i));

		if(sonContiguos(unHueco,otroHueco))
		{
			t_hueco* huecoUnificado = unificar(unHueco,otroHueco);

			list_remove_and_destroy_element(huecos,(i),(void*)t_hueco_eliminar);
			list_remove_and_destroy_element(huecos,(i-1),(void*)t_hueco_eliminar);

			list_add_in_index(huecos,--i,huecoUnificado);

			unHueco = list_get(huecos,i);

			cantidadHuecos = list_size(huecos);

		}else{

			unHueco=otroHueco;


		}



	}

}


int sonContiguos(t_hueco* unHueco,t_hueco* otroHueco)
{
	return (unHueco->paginaInicio + unHueco->cantidadPaginas) == otroHueco->paginaInicio;



}

t_hueco* unificar(t_hueco* unHueco,t_hueco* otroHueco)
{
	t_hueco* huecoUnificado = malloc(sizeof(t_hueco));

	huecoUnificado->paginaInicio = unHueco->paginaInicio;
	huecoUnificado->cantidadPaginas = unHueco->cantidadPaginas + otroHueco->cantidadPaginas ;


	return huecoUnificado;
}


void t_particion_limpiar(t_particion* particion,int comienzo,int cantidad)
{
	int i ;
	t_protoc_escrituraProceso* pedido = malloc(sizeof(t_protoc_escrituraProceso));
	pedido->tamanio = particion->pagina_tamanio;
	pedido->contenido=malloc(particion->pagina_tamanio);
	memset(pedido->contenido,'\0',pedido->tamanio);

	for(i=0; i<cantidad;i++)
	{
	pedido->pagina = comienzo+i;
		t_particion_escribirPagina(particion,pedido->pagina,pedido);
	}

	free(pedido->contenido);
	free(pedido);
}
