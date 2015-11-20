/*
 * Swap.h
 *
 *  Created on: 3/11/2015
 *      Author: utnso
 */

#ifndef SWAP_H_
#define SWAP_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/temporal.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "servidor.h"
#include "protocolos.h"
#include "particionSwap.h"
#include <stdlib.h>
#include "logueoSwap.h"
#include "swapConfig.h"


typedef struct
{
	int pid;
	int paginaComienzo;
	int cantidad;

} t_proceso ;




t_proceso* t_proceso_crear(int pid, int paginaInicio, int cantidad);
void t_proceso_eliminar(t_proceso*unProceso);




#endif /* SWAP_H_ */
