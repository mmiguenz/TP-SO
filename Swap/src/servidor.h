/*
 * servidor.h
 *
 *  Created on: 2/11/2015
 *      Author: utnso
 */

#ifndef SERVIDOR_H_
#define SERVIDOR_H_


typedef struct addrinfo addrinfo;

struct addrinfo configAddrSvr();

int getListeningSocket(struct addrinfo* listSockInfo);

int connectToClient(int listenningSocket);



#endif /* SERVIDOR_H_ */
