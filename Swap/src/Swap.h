/*
 * Swap.h
 *
 *  Created on: 3/11/2015
 *      Author: utnso
 */

#ifndef SWAP_H_
#define SWAP_H_

#include <stdlib.h>


typedef struct
{
	int pid;
	int paginaComienzo;
	int cantidad;

} t_proceso ;



t_proceso* t_proceso_crear(int pid, int paginaInicio, int cantidad)
{
	t_proceso* unproceso  = malloc(sizeof(t_proceso));

	unproceso ->cantidad= cantidad;
	unproceso->pid = pid;
	unproceso->paginaComienzo = paginaInicio;

	return unproceso;

}

void t_proceso_eliminar(t_proceso*unProceso)
{
	free(unProceso);


}




#endif /* SWAP_H_ */
