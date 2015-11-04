/*
 * particionSwap.h
 *
 *  Created on: 11/10/2015
 *      Author: utnso
 */
#include<stdio.h>
#include "swapConfig.h"
#include <string.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#ifndef PARTICIONSWAP_H_
#define PARTICIONSWAP_H_



typedef struct particion {
	FILE* archivoParticion;
	int archivo_tamanio;
	int pagina_tamanio;
	int paginas_cantidad;
	t_list* espacioLibre;


} t_particion;

typedef struct hueco
{
	int paginaInicio;
	int cantidadPaginas;

} t_hueco ;





t_particion* t_particion_crear(t_swapConfig*);
void* t_particion_leerPagina(t_particion*,int numeroDePagina);

//Retorna el nroDePAginaDondeComienzaElBloqueReservado
int t_particion_reservarPaginas(t_particion*, int cantidadDePaginas,t_list* espacioUtilizado_lista);


t_hueco* t_hueco_crear(int paginaInicio, int cantidadDePaginas);
void t_hueco_eliminar(t_hueco*);
void t_hueco_agregar(t_particion* particion,int paginaComienzo,int cantidad);



#endif /* PARTICIONSWAP_H_ */
