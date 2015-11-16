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
#include "protocolos.h"
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>


int conectar_cliente(int puerto,char* ip){
	struct sockaddr_in dire_serv;
	dire_serv.sin_family = AF_INET;
	dire_serv.sin_addr.s_addr = inet_addr(ip);
	dire_serv.sin_port = htons(puerto);
	int planificador = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(planificador, (void*) &dire_serv, sizeof(dire_serv)) > 0) {
		perror("No se pudo conectar %s");
	}
	return planificador	;
}

char* recibirMensaje(int socket) {
	char* mensaje;
	mensaje = malloc(100);
	if (recv(socket, mensaje, 100, 0) > 0){
	}else 	{
		printf("Falle");
	}
	return mensaje;
}


void enviarMesaje(int socket,char* mensaje) {
	send(socket, mensaje,strlen (mensaje)+1,0);
	printf("Envie mensaje: %s \n", mensaje);
	return;
}

//Para la memoria
void enviarSolicitud (int pid, char instruccion, int nroPag, int socket){

	/*header.msgtype = instruccion;
	 * header.msgtype = 2;leer
	 * header.msgtype = 3;escribir
	 * header.msgtype =5;entrada salida
	 * header.msgtype = 4;finalizar
	 * header.pagina = nroPag;
	 * */

	void* mensaje;
	mensaje= malloc(sizeof(char)+sizeof(int)+sizeof(int));
	int offset=0;

	t_protoc_inicio_lectura_Proceso* header = malloc(sizeof(t_protoc_inicio_lectura_Proceso));
	header->tipoInstrucc = instruccion;
	header->paginas= nroPag;
	header->pid = pid;

	memcpy(mensaje +offset  , &(header->tipoInstrucc), sizeof(char));
	offset+=sizeof(char);
	memcpy(mensaje +offset  , &(header->paginas), sizeof(int));
	offset+=sizeof(int);
	memcpy(mensaje +offset  , &(header->pid), sizeof(int));
	offset+=sizeof(int);

	send(socket,mensaje,offset,0);
	free(mensaje);
	free(header);
	return;
}

//la memoria nos avisa si realizo la operacion con exito
char* recibirMsjMemoria(int memoria){
	char* msjMemoria = malloc(sizeof(char));
	recv(memoria,msjMemoria,sizeof(char),0);
	return msjMemoria;
}

char  mandarMsjEscribir(int memoria, char texto[],int pid, char instruccion, int nroPag){
	int tamanio= strlen(texto)+1;
	void* mensaje;
	mensaje= malloc(sizeof(char)+sizeof(int)+sizeof(int)+sizeof(int)+tamanio);

	int offset=0;


	t_protoc_escrituraProceso* header = malloc(sizeof(t_protoc_escrituraProceso));
	header->tipoInstrucc= instruccion;
	header->pid= pid;
	header->pagina =nroPag;
	header->tamanio=tamanio;
	header->contenido =texto;



	memcpy(mensaje +offset  , &(header->tipoInstrucc), sizeof(char));
	offset+=sizeof(char);
	memcpy(mensaje +offset  , &(header->pid), sizeof(int));
	offset+=sizeof(int);
	memcpy(mensaje +offset  , &(header->pagina), sizeof(int));
	offset+=sizeof(int);
	memcpy(mensaje +offset  , &(header->tamanio), sizeof(int));
	offset+=sizeof(int);
	memcpy(mensaje +offset  , header->contenido, strlen(texto)+1);
	offset+=strlen(texto)+1;

	send(memoria,mensaje,offset,0);
	free(mensaje);
	free(header);

	char respuesta ;
	recv(memoria,&respuesta,sizeof(char),0);

	return respuesta;
}


char* enviarSolicitudLectura(int pid ,int  instruccion, int  paginas ,int  sMemoria)
{

	void* mensaje;
	mensaje= malloc(sizeof(char)+sizeof(int)+sizeof(int));
	int offset=0;

	t_protoc_inicio_lectura_Proceso* header = malloc(sizeof(t_protoc_inicio_lectura_Proceso));
	header->tipoInstrucc = instruccion;
	header->paginas= paginas;
	header->pid = pid;

	memcpy(mensaje +offset  , &(header->tipoInstrucc), sizeof(char));
	offset+=sizeof(char);
	memcpy(mensaje +offset  , &(header->paginas), sizeof(int));
	offset+=sizeof(int);
	memcpy(mensaje +offset  , &(header->pid), sizeof(int));
	offset+=sizeof(int);

	send(sMemoria,mensaje,offset,0);
	free(mensaje);
	free(header);

	int tamanioContenido;
	int control;

	 control = recv(sMemoria,&tamanioContenido,sizeof(int),0);
	 	 if(control<0)
	 		 goto error;

	 if (tamanioContenido != 0){
	 void* bufferLectura = malloc(tamanioContenido);
	 control= recv(sMemoria,bufferLectura,tamanioContenido,0);

	 	 if(control<0)
	 		 goto error;

	 	printf("bytes recibidos:  %d \n",tamanioContenido);
	 	 char * contenido = malloc(tamanioContenido);
	 	 memcpy(contenido,bufferLectura,tamanioContenido);

	 	 free(bufferLectura);

	 	contenido[tamanioContenido]='\0';
	 	 return  contenido;
	 }
	 else{
		 char* contenido = malloc(sizeof(int));
		 contenido = string_itoa(tamanioContenido);
		 return contenido;
	 }

	 error:
	 {
		 perror("error al recibir datos de lectura");
		 return (char*)NULL;

	 }

}



#endif /* CLIENTE_C_ */
