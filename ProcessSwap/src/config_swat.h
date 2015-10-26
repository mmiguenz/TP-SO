/*
 * config_swat.h
 *
 *  Created on: 25/10/2015
 *      Author: utnso
 */

#ifndef CONFIG_SWAT_H_
#define CONFIG_SWAT_H_

typedef struct swapConfig {
	char* puerto_Escucha;
	char* nombre_Swap;
	unsigned int cantidad_Paginas;
	unsigned int tamanio_Pagina;
	unsigned int retardo_SWAP;
	unsigned int retardo_Compact;
} t_swapConfig;

t_swapConfig* swapConfig_Create();

void swapConfig_GetConfig();

void swapConfig_Free();


#endif /* CONFIG_SWAT_H_ */
