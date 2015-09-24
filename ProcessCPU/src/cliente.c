/*
 * cliente.c
 *
 *  Created on: 17/9/2015
 *      Author: utnso
 */

#ifndef CLIENTE_C_
#define CLIENTE_C_
#include <sys/socket.h>
#include "cliente.h"
#include <commons/config.h>

int conectar_cliente(int puerto,char* ip){

	struct sockaddr_in dire_serv;
	fd_set rfds;
	dire_serv.sin_family = AF_INET;
	dire_serv.sin_addr.s_addr = inet_addr(ip);
	dire_serv.sin_port = htons(puerto);

			int planificador = socket(AF_INET, SOCK_STREAM, 0);
			if (connect(planificador, (void*) &dire_serv, sizeof(dire_serv)) == 0) {
							perror("No se pudo conectar");

			}

			return planificador	;


}
char* recibirMensaje(int socket) {

		char* mensaje;
		mensaje = (char*)malloc(sizeof(mensaje));
		recv(socket, mensaje,100,0);
		printf("Recibi mensaje: %s \n", mensaje);

		return mensaje;

}
void enviarMesaje(int socket,char* mensaje) {

	send(socket, mensaje,strlen (mensaje),0);
	printf("Envie mensaje: %s \n", mensaje);

	return;
}



#endif /* CLIENTE_C_ */
