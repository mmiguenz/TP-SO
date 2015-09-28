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

int procesarCadena(char* cadena, int memoria, int planificador, t_log* logger,char* nombreProc);
void abrir(char* path, int memoria, int planificador,t_log* logger,char* nombreProc);
void* conectar();

#endif /* HILITO_H_ */



