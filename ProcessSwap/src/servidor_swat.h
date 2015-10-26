/*
 * servidor_swat.h
 *
 *  Created on: 25/10/2015
 *      Author: utnso
 */

#ifndef SERVIDOR_SWAT_H_
#define SERVIDOR_SWAT_H_

typedef struct addrinfo addrinfo;

struct addrinfo configAddrSvr();
int getListeningSocket(struct addrinfo* listSockInfo);
int connectToClient(int listenningSocket);

#endif /* SERVIDOR_SWAT_H_ */
