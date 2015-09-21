/*
 * servidor.h
 *
 *  Created on: 14/9/2015
 *      Author: utnso
 */

#ifndef SERVIDOR_H_
#define SERVIDOR_H_


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

void *get_in_addr(struct sockaddr *sa);

void limpiar (char *cadena);

void conectar_servidor(char* puerto_escucha_planif);

#endif /* SERVIDOR_H_ */
