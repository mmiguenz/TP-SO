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

// Crea el archivo de inicializacion de Swap, lo inicializa con barra ceros
// y devuelve el path del archivo creado.
char* crearArchivoSwap(char *nombre_Swap ,int tam_Pag,int cant_Pag)
{
	char* pathArchivo=string_new();
	char* archivoFormatCero= string_from_format("dd if=/dev/zero of=/home/utnso/%s bs=%lu count=%lu",nombre_Swap,tam_Pag, cant_Pag);

	if(system(archivoFormatCero)){
		perror("Error al crear el Archivo Swap: func crearArchivoSwap en LibSwap.c");
		return EXIT_FAILURE;
	}else {
		pathArchivo=string_from_format("dd if=/dev/zero of=/home/utnso/%s",nombre_Swap);
	}

	return  pathArchivo;
}


// Crea un lista de espacios Libres. iniciliaza la particion
// y la agrega a la lista de espacios libres
// Devolviendo la lista de espacio libres.
t_list* crear_ListaLibre(int cant_Paginas)
{
	t_espacio_libre* particion;
	particion->comienzo = 1;
	particion->cant_paginas = cant_Paginas;
	t_list* lista_Libre = list_create();
	list_add(lista_Libre, particion);
	return lista_Libre;
}

// ejemplo del transform de list_maps
//char* _get_name(t_person* person) {
//                    return person->name;
//                }

t_list* crear_ListaOcupados()
{
	t_list* lista_Ocupados = list_create();
	return lista_Ocupados;
}






t_espacio_libre*  encontrar_Espacio(t_list* list_Libre, int paginas)
	{
		bool hay_Espacio(t_espacio_libre* espacio) {
			return (espacio->cant_paginas >= paginas);
		}

		t_espacio_libre* espacio;


		espacio=list_find( list_Libre,(void*) hay_Espacio);
		if(espacio == NULL)
		{
			printf("No hay Espacio");

		}
		return espacio;
	}


t_espacio_ocupado* recibir_Solicitud(PROCESOSWAP procesoSwap,t_list* list_Libres,t_list* list_Ocupados)
	{
		int paginas_requeridas=procesoSwap.pagina;
		int pid=procesoSwap.pid;
		switch ( procesoSwap.msgtype ) {
		case 1://Iniciar
			if(total_Libres(list_Libres)>=paginas_requeridas){
				t_espacio_libre* espacio = encontrar_Espacio(list_Libres, paginas_requeridas);
				if(espacio == NULL)
					{
						// ver la funcionalidadde compactar y rebuscar el espacio libre
					}
				else
					{
						return asignar_espacio_actualizar(pid,paginas_requeridas,espacio,list_Libres,list_Ocupados);
					}
			}
		  break;
		/*case 2://Leer
			//return t_espacio_ocupado;
		  break;
		case 3://finalizar
			//return t_espacio_ocupado;
		  break;
		default:
			//return t_espacio_ocupado;
		  break;

		}
		return t_espacio_ocupado;
		*/

	}
}

// devuelve el total de espacio libre de la lista
int total_Libres(t_list* espacio_Libre)
{
	//funcion que devuelve la catidad de pagina de una lista libre.
	int fun_pag(t_espacio_libre* espacio)
	{
		return espacio->cant_paginas;
	}

	t_list* listPag= list_map(espacio_Libre, (void*) *fun_pag);
	int count_libre=0;
	while(listPag->head!=NULL)
	{
		count_libre +=(int)listPag->head->data;
		listPag->head= listPag->head->next;

	}
	list_destroy(listPag);
	return count_libre;
}


t_espacio_ocupado* asignar_espacio_actualizar(pid_t pid, int paginas,t_espacio_libre* espacio, t_list* list_libre,t_list* list_Ocupado)
{
	t_espacio_ocupado* proceso_enCurso=malloc(sizeof(t_espacio_ocupado));
	proceso_enCurso-> pid = pid;
	proceso_enCurso-> comienzo = espacio->comienzo;
	proceso_enCurso-> cant_paginas = espacio->cant_paginas;

	list_add(list_Ocupado, proceso_enCurso);

	bool validar_Espacio (t_espacio_libre* hueco)
	{

		return (hueco->cant_paginas == espacio->cant_paginas && hueco->comienzo == espacio->comienzo);
	}
	if(paginas == espacio->cant_paginas)
	{
		list_remove_by_condition(list_libre,(void*)validar_Espacio);
	}
	else
	{
		espacio->comienzo = espacio->comienzo+paginas;
		espacio->cant_paginas = espacio->cant_paginas-paginas;
	}

	return proceso_enCurso;
}




