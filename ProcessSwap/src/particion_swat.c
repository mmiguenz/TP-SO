/*
 * particion_swat.c
 *
 *  Created on: 25/10/2015
 *      Author: utnso
 */
//t_list* procesosEnSwap;

#include <stdlib.h>
#include "config_swat.h"
#include "particion_swat.h"
#include <stdio.h>

t_particion* t_particion_crear(t_swapConfig* config_swat)
{
	t_particion* particion  = malloc(sizeof(t_particion));
	//procesosEnSwap = list_create();

	char* script= malloc(100);
	char* cantidad_Paginas= malloc(4);
	char* tamanio_pagina = malloc(4);
	char* nombre_particion_swat = malloc(4);

	itoa(config_swat->cantidad_Paginas,cantidad_Paginas,10);
	itoa(config_swat->tamanio_Pagina,tamanio_pagina,10);
	itoa(config_swat->nombre_Swap,nombre_particion_swat,10);

	///home/utnso/git/tp-2015-2c-cascanueces/ProcessSwap/Debug

	strcpy(script,"dd if=/dev/zero of=/home/utnso/");
	strcat(script,nombre_particion_swat);
	strcat(script," bs=");
	strcat(script,tamanio_pagina);
	strcat(script," count=");
	strcat(script,cantidad_Paginas);

	if(system(script)){
		perror("Error al crear el Archivo Swap: func system(script) en particion_swat.c");
		return EXIT_FAILURE;
	}

	particion->archivoParticion = fopen(nombre_particion_swat,"r+b");
	particion->archivo_tamanio = config_swat->cantidad_Paginas * config_swat->tamanio_Pagina ;
	particion->pagina_tamanio= config_swat->tamanio_Pagina ;
	particion->paginas_cantidad= config_swat ->cantidad_Paginas;


	return particion;
}

itoa (int value, char *result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}






























