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
#include <commons/log.h>

typedef struct  {
	int msgtype;
	int pagina;
	int pid;
}t_msgHeaderMemoria;

typedef struct  {
	int instruccion;
	int pagina;
	int pid;
	char texto[20];

}estructuraEscribir;

int conectar_cliente(int puerto,char* ip){
	struct sockaddr_in dire_serv;
	dire_serv.sin_family = AF_INET;
	dire_serv.sin_addr.s_addr = inet_addr(ip);
	dire_serv.sin_port = htons(puerto);
	int planificador = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(planificador, (void*) &dire_serv, sizeof(dire_serv)) > 0) {
		//log_info(logger, "No se pudo conectar");
		perror("No se pudo conectar %s");

		//}else{
		//		log_info(logger, "Se conecto a %s");
	}
	return planificador	;
}

char* recibirMensaje(int socket,t_log* logger) {
	char* mensaje;
	mensaje = malloc(100);
	if (recv(socket, mensaje, 100, 0) > 0){
		//printf("Recibi mensaje: %s \n", mensaje);
	}else 	{
		printf("Falle");
	}
	return mensaje;
}
void enviarMesaje(int socket,char* mensaje,t_log* logger) {
	send(socket, mensaje,strlen (mensaje)+1,0);
	printf("Envie mensaje: %s \n", mensaje);
	return;
}

void enviarSolicitud (int pid, int instruccion, int nroPag, int socket){
	t_msgHeaderMemoria header;
	memset(&header, 0, sizeof(t_msgHeaderMemoria));

	header.msgtype = instruccion;
	/*header.msgtype = 2;leer
	 * header.msgtype = 3;escribir
	 * header.msgtype =5;entrada salida
	 * header.msgtype = 4;finalizar
	 * */
	header.pagina = nroPag;
	header.pid = pid;
	printf("Los mensajes enviados a memoria son: \n");
	printf("Pagina %d\n",header.pagina);
	printf("Tipo de instruccion  %d \n",header.msgtype);
	printf("PID de proc %d \n ",header.pid);

	send(socket,&header,sizeof(t_msgHeaderMemoria),0);
	return;
}
int recibirMsjMemoria(int memoria){

	PROCESO mensaje;

	memset(&mensaje, 0, sizeof(PROCESO));


	recv(memoria, &mensaje, sizeof(PROCESO), 0);


	//recv(memoria, &proceso, sizeof(proceso), 0);

	return mensaje.aceptado;
}

void mandarMsjEscribir(int memoria, char texto[20],int pid, int instruccion, int nroPag){

		char* mensaje;
		mensaje= malloc(sizeof(int)+sizeof(int)+sizeof(int)+sizeof(int)+strlen(texto)+1);

		int offset=0;
		memcpy(mensaje +offset  , &instruccion, sizeof(int));
		offset+=sizeof(int);
		memcpy(mensaje +offset  , &pid, sizeof(int));
		offset+=sizeof(int);
		memcpy(mensaje +offset  , &nroPag, sizeof(int));
		offset+=sizeof(int);
		memcpy(mensaje +offset  , texto, strlen(texto)+1);
		offset+=strlen(texto)+1;

		/*memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
		header.msgtype = 1;//MSG_PCB;
		header.payload_size = offset;
		send(socketCliente,&header,sizeof(header),0);
		send(socketCliente,mensaje,header.payload_size,0);*/

		send(memoria,mensaje,offset,0);


		free(mensaje);
		return;

}

#endif /* CLIENTE_C_ */
