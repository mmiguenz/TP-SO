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
#include "protocolos.h"
#include <unistd.h>
#include <commons/log.h>

#ifndef PARTICIONSWAP_H_
#define PARTICIONSWAP_H_



typedef struct particion {
	FILE* archivoParticion;
	int archivo_tamanio;
	int pagina_tamanio;
	int paginas_cantidad;
	t_list* espacioLibre;
	unsigned int retardoCompactacion;
	t_log* logSwap;


} t_particion;

typedef struct hueco
{
	int paginaInicio;
	int cantidadPaginas;

} t_hueco ;











t_particion* t_particion_crear(t_swapConfig*,t_log* logSwap);
void* t_particion_leerPagina(t_particion* particion ,int numeroDePagina);
void t_particion_escribirPagina(t_particion* particion ,int numeroDePagina, t_protoc_escrituraProceso* pedido);

//Retorna el nroDePAginaDondeComienzaElBloqueReservado
int t_particion_reservarPaginas(t_particion*, int cantidadDePaginas,t_list* espacioUtilizado_lista);

//Escribe \0 en todas las posiciones que libera un proeceso al finaliar
void t_particion_limpiar(t_particion* particion, int,int);

t_hueco* t_hueco_crear(int paginaInicio, int cantidadDePaginas);
void t_hueco_eliminar(t_hueco*);
void t_hueco_agregar(t_particion* particion,int paginaComienzo,int cantidad);



#endif /* PARTICIONSWAP_H_ */
