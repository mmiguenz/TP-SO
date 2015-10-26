/*
 * particion_swat.h
 *
 *  Created on: 25/10/2015
 *      Author: utnso
 */
#include<stdio.h>
#ifndef PARTICION_SWAT_H_
#define PARTICION_SWAT_H_


typedef struct particion {
	FILE* archivoParticion;
	int archivo_tamanio;
	int pagina_tamanio;
	int paginas_cantidad;

} t_particion;


t_particion* t_particion_crear(t_swapConfig* config_swat);
#endif /* PARTICION_SWAT_H_ */
