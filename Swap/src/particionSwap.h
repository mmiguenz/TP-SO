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
#include <commons/collections/dictionary.h>
#ifndef PARTICIONSWAP_H_
#define PARTICIONSWAP_H_



typedef struct particion {
	FILE* archivoParticion;
	int archivo_tamanio;
	int pagina_tamanio;
	int paginas_cantidad;
	char** bitMapPaginasOcupadas;



} t_particion;


t_particion* t_particion_crear(t_swapConfig*);
void* t_particion_leerPagina(t_particion*,int numeroDePagina);

//Retorna el nroDePAginaDondeComienzaElBloqueReservado
int t_particion_reservarPaginas(t_particion*, int cantidadDePaginas);

#endif /* PARTICIONSWAP_H_ */
