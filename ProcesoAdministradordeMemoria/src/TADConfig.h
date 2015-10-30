/*
 * TADConfig.h
 *
 *  Created on: 19/10/2015
 *      Author: utnso
 */

#ifndef TADCONFIG_H_
#define TADCONFIG_H_

#include <commons/config.h>


typedef struct {
	char* puerto_escucha;
	char* ip_swap;
	char* puerto_swap;
	int max_marcos_proceso;
	int cantidad_marcos;
	int tamanio_marco;
	int entradas_TLB;
	char* tlb_habilitada;
	int retardo_memoria;
	char* algoritmo_reemplazo;
}t_paramConfigAdmMem;


t_paramConfigAdmMem* establecerConfigMemoria(t_config*);

#endif /* TADCONFIG_H_ */
