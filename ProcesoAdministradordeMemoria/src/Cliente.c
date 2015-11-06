


#ifndef CLIENTE_C_
#define CLIENTE_C_
#include <sys/socket.h>
#include "Cliente.h"
#include <commons/config.h>
#include "servidor.h"

int conectar_cliente(char* puerto,char* ip){
	struct sockaddr_in dire_serv;
	dire_serv.sin_family = AF_INET;
	dire_serv.sin_addr.s_addr = inet_addr(ip);
	dire_serv.sin_port = htons(atoi(puerto));
	int swap = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(swap, (void*) &dire_serv, sizeof(dire_serv)) < 0) {
		perror("Error al intentar conectar al módulo Swap");
		exit(0);
	}
	else printf("Conexión con Swap establecida \n");

	return swap	;


}

void recibirMensaje(int socket, t_espacio_ocupado* listaDePaginas) {

	if (recv(socket, listaDePaginas,100,0)> 0){
			printf("Recibi mensaje:  \n");
	}else 	{
		printf("Falle");
	}
	//return mensaje;
 }

void enviarMensaje(int socket,t_msgHeaderMemoria encabezado) {
	send(socket, &encabezado,sizeof(t_msgHeaderMemoria)+1,0);
	printf("Envie mensaje a swap  \n");
	return;
}

#endif

