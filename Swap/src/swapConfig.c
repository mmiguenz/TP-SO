/*
 * swapConfig.c
 *
 *  Created on: 6/10/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <commons/config.h>
#include <stdlib.h>
#include "swapConfig.h"

t_swapConfig* swapConfig_Create(){
	t_swapConfig* swapConfig = malloc(100);
	swapConfig->puerto_Escucha = malloc(7);
	swapConfig->nombre_Swap = malloc(30);

	return swapConfig;
}

void swapConfig_GetConfig(t_swapConfig* swapConfig){

	t_config* fileCfg = config_create("configSWAP");

	if(!fileCfg)
	{
		perror("Error al leer archivo de configuracion SWAP");

	}





	strcpy(swapConfig->puerto_Escucha,config_get_string_value(fileCfg,"PUERTO_ESCUCHA"));
	strcpy(swapConfig->nombre_Swap,config_get_string_value(fileCfg,"NOMBRE_SWAP"));
	swapConfig->cantidad_Paginas = config_get_int_value(fileCfg,"CANTIDAD_PAGINAS");
	swapConfig->tamanio_Pagina = config_get_int_value(fileCfg,"TAMANIO_PAGINA");
	swapConfig->retardo_SWAP = (int) (config_get_double_value(fileCfg,"RETARDO_SWAP")) * 1000000;
	swapConfig->retardo_Compact = config_get_int_value(fileCfg,"RETARDO_COMPACTACION");

}

void swapConfig_Free(t_swapConfig* swapConfig){

	free(swapConfig->puerto_Escucha);
	free(swapConfig->nombre_Swap);
	free(swapConfig);

}
