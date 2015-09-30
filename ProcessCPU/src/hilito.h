/*
 * hilito.h
 *
 *  Created on: 19/9/2015
 *      Author: utnso
 */

#ifndef HILITO_H_
#define HILITO_H_


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <commons/string.h>
#include <assert.h>
#include <commons/log.h>

typedef struct  {
	int msgtype;
	int payload_size;
}t_msgHeader;

typedef struct {
char* nombreProc;
int estado;
int PID;
int contadorProgram;
char* path;
int cpu_asignada;
}PCB ;

int procesarCadena(char* cadena, int memoria, int planificador, t_log* logger,PCB* PcbAux);
void abrir(PCB* PcbAux, int memoria, int planificador,t_log* logger);
void* conectar();

#endif /* HILITO_H_ */



