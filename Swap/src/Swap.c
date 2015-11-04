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
#include <commons/collections/list.h>
#include "servidor.h"
#include "protocolos.h"
#include "swapConfig.h"
#include "particionSwap.h"
#include "Swap.h"



#define BACKLOG 5




void atenderPedido(int memSocket, void* buffer );
void iniciarProceso(int memSocket);
void realizarLectura(int memSocket);
void realizarEscritura(int memSocket);
void finalizarProceso(int memSocket);
void tipoDePedidoIncorrecto(int memSocket);
int hayEspacio(int cantidadNuevoProceso);

t_particion* particion;
t_swapConfig* config;
t_list* espacioUtilizado_lista;

int main (){

	struct addrinfo* swapInfo;
	struct addrinfo hints;
	 config = swapConfig_Create();
	swapConfig_GetConfig(config);

	particion = t_particion_crear(config);
	espacioUtilizado_lista = list_create();

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
	char respuestaMemoria;

	if(hayEspacio(pedido->paginas))
	{


	    int paginaComienzo =  t_particion_reservarPaginas(particion,pedido->paginas,espacioUtilizado_lista);

		respuestaMemoria = 1 ;
		t_proceso* unProceso = t_proceso_crear(pedido->pid,paginaComienzo,pedido->paginas);
		list_add(espacioUtilizado_lista,(void*)unProceso);


	}else
	{
		respuestaMemoria = 0;

	}
	int error  =  send(memSocket,&respuestaMemoria,sizeof(char),0);

	if (error>=0)
		printf("Se notifico al la memoria el inicio de proceso. Resultado= %s",&respuestaMemoria);
	else
		perror("Error Al comunicarse con Memoria");


	free(pedido);


}
void realizarLectura(int memSocket)
{



}

void realizarEscritura(int memSocket)
{



}




void finalizarProceso(int memSocket)
{
	// Recuperar Datos PEDIDO

	t_protoc_Finaliza* pedido = malloc(sizeof(t_protoc_Finaliza));
	pedido->tipoInstrucc = FINALIZAR;

	recv(memSocket,&pedido->pid,sizeof(int),0);


	bool buscarPid(t_proceso* proc)
	{
		return proc->pid==pedido->pid;

	}


	t_proceso* proceso = list_remove_by_condition(espacioUtilizado_lista,(void*)buscarPid);

	t_hueco_agregar(particion,proceso->paginaComienzo,proceso->cantidad);

	char respuestaMemoria = 1 ;

	int error = send(memSocket,&respuestaMemoria,sizeof(char),0);

	if(error<0)
	{
		perror("Error al notificar finalizacion de proceso a memoria");

	}
	free(pedido);
	free(proceso);

}
void tipoDePedidoIncorrecto(int memSocket)
{


}


int hayEspacio(int cantidadNuevoProceso)
{
	int obtenerPaginasOcupadas(t_proceso* unProceso)
	{
		return unProceso->cantidad;

	}



	t_list* listaAux = list_map(espacioUtilizado_lista,(void*)obtenerPaginasOcupadas);

	int i ;
	int espacioUtilizado=0;
	for(i=0; i < list_size(listaAux); i++)
	{
		int espacio;
		memcpy(&espacio,list_get(listaAux,i),sizeof(int));
		espacioUtilizado+= espacio;

	}


	return (espacioUtilizado + cantidadNuevoProceso) <= particion->archivo_tamanio;


}


int calcularPaginaEnSwap(int pid, int pagina)
{

	bool buscarPid(t_proceso* proc)
	{
		return proc->pid==pid;

	}

	t_proceso* proceso = list_find(espacioUtilizado_lista,(void*)buscarPid);

	return (proceso->paginaComienzo + pagina );


}



t_proceso* t_proceso_crear(int pid, int paginaInicio, int cantidad)
{
	t_proceso* unproceso  = malloc(sizeof(t_proceso));

	unproceso ->cantidad= cantidad;
	unproceso->pid = pid;
	unproceso->paginaComienzo = paginaInicio;

	return unproceso;

}

void t_proceso_eliminar(t_proceso*unProceso)
{
	free(unProceso);


}
