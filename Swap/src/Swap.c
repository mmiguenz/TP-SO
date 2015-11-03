/*
 * Swap.c
 *
 *  Created on: 6/9/2015
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
#include <commons/temporal.h>
#include <commons/config.h>
#include <commons/string.h>
#include "servidor.h"
#include "protocolos.h"
#include "swapConfig.h"
#include "particionSwap.h"



#define BACKLOG 5

void atenderPedido(int memSocket, void* buffer );
void iniciarProceso(int memSocket);
void realizarLectura(int memSocket);
void realizarEscritura(int memSocket);
void finalizarProceso(int memSocket);
void tipoDePedidoIncorrecto(int memSocket);

t_dictionary* tablaPidsEnSwap;
t_particion* particion;
t_swapConfig* config;

int main (){

	struct addrinfo* swapInfo;
	struct addrinfo hints;
	 config = swapConfig_Create();
	swapConfig_GetConfig(config);

	particion = t_particion_crear(config);
	tablaPidsEnSwap = dictionary_create();

	hints = configAddrSvr();

	getaddrinfo(NULL,config->puerto_Escucha,&hints,&swapInfo);

	int listenningSocket =getListeningSocket(swapInfo);

	listen(listenningSocket,BACKLOG);		// IMPORTANTE: listen() es una syscall BLOQUEANTE.

	char *tiempo=temporal_get_string_time();
	printf("Se ha iniciado el SWAP a las %s.\n",tiempo);
	free(tiempo);

	int memSocket = connectToClient(listenningSocket);


	int status = 1;
	size_t packageSize = sizeof(char);
	void* buffer = malloc(packageSize);

	while (status != 0) {

		status = recv(memSocket,buffer,packageSize,0);
		if (status != 0){

			 atenderPedido(memSocket,buffer);


		}




	}
	if (status == -1) {
		printf("Se ha cerrado el SWAP porque la Memoria ha finalizado");
	}

	close(memSocket);
	close(listenningSocket);
	swapConfig_Free(config);

	return 0;

}




void atenderPedido(int memSocket, void* buffer )
{

	char tipoPedido;
	memcpy(&tipoPedido,buffer,sizeof(char));


	switch (tipoPedido)
	{
	case    INICIAR: iniciarProceso(memSocket); break;
	case   	 LEER: realizarLectura(memSocket); break ;
	case 	ESCRIBIR: realizarEscritura(memSocket); break ;
	case  	 FINALIZAR: finalizarProceso(memSocket); break;

	default: tipoDePedidoIncorrecto(memSocket); break;

	}





}




void iniciarProceso(int memSocket)
{
	// Recuperar Datos PEDIDO

	t_protoc_inicio_lectura_Proceso* pedido = malloc(sizeof(t_protoc_inicio_lectura_Proceso));
	pedido->tipoInstrucc = INICIAR;

	recv(memSocket,&pedido->paginas,sizeof(int),0);
	recv(memSocket,&pedido->pid,sizeof(int),0);

	int paginaComienzo =  t_particion_reservarPaginas(particion,pedido->paginas);

	char respuestaMemoria;



	if(paginaComienzo>=0)
	{
		dictionary_put(tablaPidsEnSwap,string_itoa(pedido->pid),&paginaComienzo);
		respuestaMemoria = 1 ;


	}else
	{
		respuestaMemoria = 0;

	}




	int error  =  send(memSocket,&respuestaMemoria,sizeof(char),0);

	if (error>=0)
		printf("Se notifico al la memoria el inicio de proceso. Resultado= %s",&respuestaMemoria);
	else
		perror("Error Al comunicarse con Memoria");





}
void realizarLectura(int memSocket)
{



}

void realizarEscritura(int memSocket)
{



}




void finalizarProceso(int memSocket)
{





}
void tipoDePedidoIncorrecto(int memSocket)
{


}




