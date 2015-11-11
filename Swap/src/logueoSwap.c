/*
 * logueoSwap.c
 *
 *  Created on: 11/11/2015
 *      Author: utnso
 */


#include "logueoSwap.h"



t_SwapLog* t_swapLog_crear(int pid ,int byteInicial, int tamanio, char* contenido)
{
	t_SwapLog* swapLog = malloc(sizeof(t_SwapLog));


	swapLog->pid=pid;
	swapLog->byteInicial= byteInicial;
	swapLog->tamanio= tamanio;
	swapLog->contenido = NULL;
	if(contenido!=NULL)
	{
		swapLog->contenido= malloc(tamanio);
		memcpy(swapLog->contenido,contenido,tamanio);
	}
	return swapLog;

}
void t_swapLog_eliminar(t_SwapLog* swapLog)
{
	free(swapLog->contenido);
	free(swapLog);


}



void t_loguear(t_log* archivoLog,tiposLog tipoLog, t_SwapLog* swapLog)
{

	switch(tipoLog)
	{
	case ASIGNADO: logAsignado(archivoLog,swapLog);break;
	case LIBERADO: logLiberado(archivoLog,swapLog);break;
	case RECHAZADO: logRechazado(archivoLog,swapLog);break;
	case COMPATACION: logCompactacion(archivoLog,swapLog);break;
	case LECTURA: logLectura(archivoLog,swapLog);break;
	case ESCRITURA: logEscritura(archivoLog,swapLog);break;

	}


}

void logAsignado(t_log* archivoLog, t_SwapLog* swapLog)
{

	char* msj  = malloc(100);
	memset(msj,'\0',100);

	sprintf(msj,"mProc asignado. || PID = %d || Byte Inicial = %d || Tamanio = %d Bytes",swapLog->pid,swapLog->byteInicial,swapLog->tamanio);
	log_info(archivoLog,msj);
	free(msj);

}


void logLiberado(t_log* archivoLog, t_SwapLog* swapLog)
{

	char* msj  = malloc(100);
	memset(msj,'\0',100);

	sprintf(msj,"mProc Liberado. || PID = %d || Byte Inicial = %d || Bytes liberados = %d",swapLog->pid,swapLog->byteInicial,swapLog->tamanio);
	log_info(archivoLog,msj);
	free(msj);


}
void logRechazado(t_log* archivoLog, t_SwapLog* swapLog)
{
	char* msj  = malloc(100);
		memset(msj,'\0',100);

		sprintf(msj,"mProc Rechazado por falta de espacio. || PID = %d ||",swapLog->pid);
		log_info(archivoLog,msj);
		free(msj);


}
void logCompactacion(t_log* archivoLog, t_SwapLog* swapLog)
{
	char* msj  = malloc(100);
			memset(msj,'\0',100);

			sprintf(msj,"Compactación iniciada/finalizada por fragmentación externa");
			log_info(archivoLog,msj);
			free(msj);


}
void logLectura(t_log* archivoLog, t_SwapLog* swapLog)
{
	char* msj  = malloc(100);
	memset(msj,'\0',100);

	sprintf(msj,"Lectura Solicitada. || PID = %d || Byte Inicial = %d || Tamanio = %d || Contenido = %s",swapLog->pid,swapLog->byteInicial,swapLog->tamanio,swapLog->contenido);
	log_info(archivoLog,msj);
	free(msj);

}
void logEscritura(t_log* archivoLog, t_SwapLog* swapLog)
{
	char* msj  = malloc(100);
	memset(msj,'\0',100);

	sprintf(msj,"Escritura Solicitada. || PID = %d || Byte Inicial = %d || Tamanio = %d || Contenido = %s",swapLog->pid,swapLog->byteInicial,swapLog->tamanio,swapLog->contenido);
	log_info(archivoLog,msj);
	free(msj);


}
