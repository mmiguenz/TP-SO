/*
 * LibSwap.h
 *
 *  Created on: 24/9/2015
 *      Author: utnso
 */

#ifndef LIBSWAP_H_
#define LIBSWAP_H_
#include <commons/collections/list.h>
#include "protocolo_swat.h"
#include <commons/log.h>


typedef struct  {
int msgtype;
int pagina;
int pid;}PROCESOSWAP;

// Tratamiento de listas
	typedef struct
	{
		 pid_t pid;
		 int comienzo;
		 int cant_paginas;
	}t_espacio_ocupado;

	typedef struct
	{
		 int comienzo;
		 int cant_paginas;
	}t_espacio_libre;

void shell(int listener, int skEmisor, int skReceptor, char * buf, int nbytes);

void *get_in_addr(struct sockaddr *sa);

void limpiar (char *cadena);

int tamaniobuf(char cad[]);

int esComando(char * comando);

char* crearArchivoSwap(char *nombre_Swap ,int tam_Pag,int cant_Pag);

t_list* crear_ListaLibre(int cant_Paginas);
t_list* crear_ListaOcupados();

int total_Libres(t_list* espacio_Libre);
t_espacio_libre*  encontrar_Espacio(t_list* list_Libre, int paginas);
t_espacio_ocupado* recibir_Solicitud(PROCESOSWAP procesoSwap,t_list* list_Libres,t_list* list_Ocupados);
t_espacio_ocupado* asignar_espacio_actualizar(int pid, int paginas,t_espacio_libre* espacio, t_list* list_libre,t_list* list_Ocupado);
t_prot_cpu_mem* desSerializar(void* buffer, size_t packageSize);
void responderPedido(int memSocket, t_prot_cpu_mem* pedido,t_list* list_Libres,t_list* list_Ocupados,t_log* log_Swap);


#endif /* LIBSWAP_H_ */
