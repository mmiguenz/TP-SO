/*
 * servidor.h
 *
 *  Created on: 21/9/2015
 *      Author: utnso
 */

#ifndef SERVIDOR_H_
#define SERVIDOR_H_
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <commons/log.h>
#include <semaphore.h>
#include <pthread.h>
#include "PCB.h"

typedef struct  {
	int msgtype;
	int payload_size;
}t_msgHeader;

typedef struct  {
	int cpu;
	float porcentaje;
}t_msgPorc;



typedef struct  {

	int contadorDePrograma;
	int tiempo;
	int pid;
}PCB_PARCIAL;



void * search_and_destroy(int pid,t_queue * running_PCB);


void* dormir(void* param_hilo);

void procesar_mensaje(int socketCliente,t_msgHeader header,t_queue * fifo_PCB, t_log* logger, t_queue * running_PCB,  t_queue * block_PCB);
void* manejo_porc();

/**
 * Abre u socket INET para atender al servicio cuyo nombre es Servicio.
 * El Servicio debe estar dado de alta en /etc/services como tcp.
 * Devuelve el descriptor del socket que atiende a ese servicio o -1 si ha habido error.
 */
int Abre_Socket_Inet (char* puerto_escucha_planif);


/**
 * Acepta un cliente para un socket INET.
 * Devuelve el descriptor de la conexión con el cliente o -1 si ha habido error.
 */
int Acepta_Conexion_Cliente (int Descriptor);

/** Lee Datos de tamaño Longitud de un socket cuyo descriptor es fd.
 * Devuelve el numero de bytes leidos o -1 si ha habido error */

int Lee_Socket (int fd, char *Datos, int Longitud);

/** Envia Datos de tamaño Longitud por el socket cuyo descriptor es fd.
 * Devuelve el número de bytes escritos o -1 si ha habido error. */

int Escribe_Socket (int fd, char *Datos, int Longitud);


void *manejo_cpu_libres(void* mensa);


#define MAX_CLIENTES 10

/* Prototipos de las funciones definidas en este fichero */
void nuevoCliente (int servidor, int *clientes, int *nClientes);
int dameMaximo (int *tabla, int n);
void compactaClaves (int *tabla, int *n);


void conectar_fifo(char* puerto_escucha_planif,t_queue * fifo_PCB, t_log* logger, t_queue * running_PCB, t_queue * block_PCB);



#endif /* SERVIDOR_H_ */
