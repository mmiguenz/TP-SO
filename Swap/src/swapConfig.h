/*
 * swapConfig.h
 *
 *  Created on: 6/10/2015
 *      Author: utnso
 */

#ifndef SWAPCONFIG_H_
#define SWAPCONFIG_H_

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


#endif /* SWAPCONFIG_H_ */
