/*
 * Cliente.h
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#include <regex.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <fcntl.h>
#include <errno.h>
#include <resolv.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>
#include <commons/config.h>
#include <commons/log.h>
#include "servidor.h"

typedef struct  {
int msgtype;
int pagina;
int contenido;}

PROCESOSWAP;



int conectar_cliente(int puerto,char* ip);
char* recibirMensaje(int socket);
void enviarMensaje(int socket,t_msgHeaderMemoria encabezado);

#endif /* CLIENTE_H_ */


