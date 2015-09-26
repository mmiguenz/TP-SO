/*
 * LibSwap.c
 *
 *  Created on: 24/9/2015
 *      Author: utnso
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
#include <commons/collections/list.h>
#include "LibSwap.h"


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


void shell(int listener, int skEmisor, int skReceptor, char * buf, int nbytes){


    printf("%s\n", buf);//action);


}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void limpiar (char *cadena)
{
  char *p;
  p = strchr (cadena, '\n');
  if (p)
    *p = '\0';
}



int tamaniobuf(char cad[])
{
   int pos = -1;
   int len = strlen( cad);
int i;
   for( i = 0; pos == -1 && i < len; i++){ // si quitas la condición pos == -1
            // te devuelve la última posición encontrada (si es que hay más de 1)
      if(*(cad+i) == '\0')
         pos = i+1;
   }
   return pos;
}

int esComando(char * comando){
	int flag ;
	if(strcmp(comando,"Correr Programa")){
		return 1;
		}
	return 0;
}


char* crearArchivoSwap(char *nombre_Swap ,long tamanio)
{
	char* pathArchivo=string_new();
	char* archivoFormatCero= string_from_format("dd if=/dev/zero of=/home/utnso/git/tp-2015-2c-cascanueces/ProcessSwap/src/%s bs=%lu count=1",nombre_Swap,tamanio);

	if(system(archivoFormatCero)){
		perror("Error al crear el Archivo Swap: func crearArchivoSwap en LibSwap.c");
		return EXIT_FAILURE;
	}else {
		pathArchivo=string_from_format("/%s",nombre_Swap);
	}

	return  pathArchivo;
}

t_list* crear_ListaLibre(int cant_Paginas)
{
 t_list* listaLibre = list_create();
 t_espacio_libre* particion;
 particion->comienzo = 1;
 particion->cant_paginas = cant_Paginas;
 list_add(listaLibre, particion);
 return listaLibre;
}

