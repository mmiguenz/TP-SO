/*
 * TADConfig.c
 *
 *  Created on: 19/10/2015
 *      Author: utnso
 */

#include "TADConfig.h"
#include <commons/config.h>
#include <stdlib.h>

t_paramConfigAdmMem* establecerConfigMemoria(){

	t_config* archConfiguracion = config_create("../config.cfg");

	t_paramConfigAdmMem* config = malloc(sizeof(t_paramConfigAdmMem));
	config->puerto_escucha = config_get_string_value(archConfiguracion,"PUERTO_ESCUCHA");
	config->puerto_swap = config_get_int_value(archConfiguracion,"PUERTO_SWAP");
	config->ip_swap = config_get_string_value(archConfiguracion,"IP_SWAP");
	config->max_marcos_proceso = config_get_int_value(archConfiguracion,"MAXIMO_MARCOS_POR_PROCESO");
	config->cantidad_marcos = config_get_int_value(archConfiguracion,"CANTIDAD_MARCOS");
	config->tamanio_marco = config_get_int_value(archConfiguracion,"TAMANIO_MARCO");
	config->entradas_TLB = config_get_int_value(archConfiguracion,"ENTRADAS_TLB");
	config->tlb_habilitada = config_get_string_value(archConfiguracion,"TLB_HABILITADA");
	config->retardo_memoria = config_get_int_value(archConfiguracion,"RETARDO_MEMORIA");
	config->algoritmo_reemplazo = config_get_string_value(archConfiguracion,"ALGORITMO_REEMPLAZO");
	free(archConfiguracion);
	return config;
}

