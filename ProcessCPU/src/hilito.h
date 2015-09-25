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
#define MAX_PACKAGE_SIZE 1024	//El servidor no admitira paquetes de mas de 1024 bytes
#define MAXUSERNAME 30
#define MAX_MESSAGE_SIZE 300

typedef struct _t_Package {
		char username[MAXUSERNAME];
		uint32_t username_long;
		char message[MAX_MESSAGE_SIZE];
		uint32_t message_long;
} t_Package;

int procesarCadena(char* cadena, int memoria, int planificador);
void abrir(char* path);
//, int memoria, int planificador
int recieve_and_deserialize(t_Package *,int);
void* conectar();

#endif /* HILITO_H_ */



