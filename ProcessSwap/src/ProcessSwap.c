/*
 ============================================================================
 Name        : ProcessSwap.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

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
#include <commons/config.h>
#include "LibSwap.h"
#include "config_swat.h"
#include "particion_swat.h"
#include "servidor_swat.h"
#include "protocolo_swat.h"
#include <commons/log.h>
#include <commons/collections/list.h>

#define BACKLOG 5

t_particion* particion;
t_swapConfig* config;


int main(void)
{
	//Logueo
	t_log* log_Swap = log_create("Log_Swap.txt","AdministradorSwap",false,LOG_LEVEL_TRACE);
	log_trace(log_Swap, "Inicia Logeo Swap.");

	//estructuras de conexion
	struct addrinfo* swapInfo;
	struct addrinfo hints;

	//configuraciones
	config = swapConfig_Create();
	swapConfig_GetConfig(config);
	particion = t_particion_crear(config);

	//manejo de listas Administradoras.
	t_list* listaEspaciosLibres = crear_ListaLibre(config->cantidad_Paginas);
	t_list* espacio_ocupado= crear_ListaOcupados();
	int espacio_libre = total_Libres(listaEspaciosLibres);

	//conexiones
	hints = configAddrSvr();
	getaddrinfo(NULL,config->puerto_Escucha,&hints,&swapInfo);
	int listenningSocket =getListeningSocket(swapInfo);

	listen(listenningSocket,BACKLOG);		// IMPORTANTE: listen() es una syscall BLOQUEANTE.


	int memSocket = connectToClient(listenningSocket);

	int status = 1;
	size_t packageSize = sizeof(t_prot_cpu_mem);
	void* buffer = malloc(packageSize);

	while (status != 0) {

		status = recv(memSocket,buffer,packageSize,0);
		if (status != 0){
			printf("Se recibio el paquete desde el administrador de memoria");
			t_prot_cpu_mem* pedido = desSerializar(buffer,packageSize);
			responderPedido(memSocket,pedido);

		}
	}

	if (status == -1) {
		printf("Se ha cerrado el SWAP porque la Memoria ha finalizado");
	}

	close(memSocket);
	close(listenningSocket);
	swapConfig_Free(config);

	return 0;


/*
	//MOCK de prueba de funcionalidad

	//creo el proceso swap que recibe desde la memoria
	PROCESOSWAP proceso_recibe;
	proceso_recibe.msgtype=1;
	proceso_recibe.pagina=1;
	proceso_recibe.pid=1;
	printf("Pagina wapppppppppppppppppppppppppppppppp: %d \n",proceso_recibe.msgtype);
	// creo la estructura a devolver
	t_espacio_ocupado* struct_paraMemoria = recibir_Solicitud(proceso_recibe,listaEspaciosLibres,espacio_ocupado);

	*/
}
