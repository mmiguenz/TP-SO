/*
 * servidor.c
 *
 *  Created on: 2/11/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <netdb.h>
#include <unistd.h>
#include "servidor.h"

struct addrinfo configAddrSvr(){

	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE;			// Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM;

	return hints ;

}

int getListeningSocket(struct addrinfo* listSockInfo){

	int lSocket = socket(listSockInfo->ai_family, listSockInfo->ai_socktype, listSockInfo->ai_protocol);

		int activated = 1;

		setsockopt(lSocket,SOL_SOCKET,SO_REUSEADDR,&activated,sizeof(activated));

		if(bind(lSocket,listSockInfo->ai_addr, listSockInfo->ai_addrlen) != 0){
			perror("Falló el bind");
			exit(EXIT_FAILURE);

		}

		freeaddrinfo(listSockInfo);

			return lSocket;

}

int connectToClient(int listenningSocket){

		struct sockaddr_in addr;			// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
		socklen_t addrlen = sizeof(addr);
		int cSocket =accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);
		if (cSocket < 0){
			perror("Fallo al conectar al cliente");
		}

		return cSocket;

}

