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



int conectar_cliente(char* puerto,char* ip);
void recibirMensaje(int socket, t_espacio_ocupado* listaDePaginas);
void enviarMensaje(int socket,t_msgHeaderMemoria encabezado);
char* enviarSolicitudLectura(int pid ,int  instruccion, int  paginas ,int  sMemoria);

#endif /* CLIENTE_H_ */


