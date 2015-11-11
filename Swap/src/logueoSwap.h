/*
 * logueoSwap.h

 *
 *  Created on: 11/11/2015
 *      Author: utnso
 */

#ifndef LOGUEOSWAP_H_
#define LOGUEOSWAP_H_

#include <commons/log.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
	int pid ;
	int byteInicial;
	int tamanio;
	char* contenido;

} t_SwapLog;

typedef enum {
	ASIGNADO,
	LIBERADO,
	RECHAZADO,
	COMPATACION,
	LECTURA,
	ESCRITURA

} tiposLog;


void t_loguear(t_log* archivoLog,tiposLog tipoLog, t_SwapLog* swapLog);
t_SwapLog* t_swapLog_crear(int pid ,int byteInicial, int tamanio, char* contenido);
void t_swapLog_eliminar(t_SwapLog* swalLog);

void logAsignado(t_log* archivoLog,t_SwapLog* swapLog);
void logLiberado(t_log* archivoLog,t_SwapLog* swapLog);
void logRechazado(t_log* archivoLog,t_SwapLog* swapLog);
void logCompactacion(t_log* archivoLog,t_SwapLog* swapLog);
void logLectura(t_log* archivoLog,t_SwapLog* swapLog);
void logEscritura(t_log* archivoLog,t_SwapLog* swapLog);




#endif /* LOGUEOSWAP_H_ */
