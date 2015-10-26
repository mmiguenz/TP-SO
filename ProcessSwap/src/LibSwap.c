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
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>



int tam_Pagina , cant_Pagina, tamanio_Total;
char* ruta_Swap;
typedef enum tipopedidosCpu {INICIAR=1,LEER,FINALIZAR,ESCRIBIR} t_tipoPedidos;

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
	tam_Pagina=tam_Pag;
	cant_Pagina=cant_Pag;
	tamanio_Total=tam_Pagina*cant_Pagina;
	if(system(archivoFormatCero)){
		perror("Error al crear el Archivo Swap: func crearArchivoSwap en LibSwap.c");
		return EXIT_FAILURE;
	}else {
		pathArchivo=string_from_format("/home/utnso/%s",nombre_Swap);
	}
	ruta_Swap=pathArchivo;
	return  pathArchivo;
}


// Crea un lista de espacios Libres. iniciliaza la particion
// y la agrega a la lista de espacios libres
// Devolviendo la lista de espacio libres.
t_list* crear_ListaLibre(int cant_Paginas)
{
	t_espacio_libre particion;

	particion.comienzo = 1;
	particion.cant_paginas = cant_Paginas;
	t_list* lista_Libre = list_create();
	list_add(lista_Libre, &particion);
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


t_espacio_ocupado* iniciarProceso(int memSocket,t_prot_cpu_mem* pedido,t_list* list_Libres,t_list* list_Ocupados)
	{
		t_espacio_ocupado* proceso_ocupado;//=malloc(sizeof(t_espacio_ocupado));
		int cant_pag_requeridas=procesoSwap.pagina;
		int pid_proceso_en_curso=procesoSwap.pid;
		int mensaje_solicitado=procesoSwap.msgtype;

		int espacio_libre=total_Libres(list_Libres);
		switch ( mensaje_solicitado) {
		case 1://Iniciar
			if(espacio_libre>=cant_pag_requeridas){
				t_espacio_libre* espacio = encontrar_Espacio(list_Libres, procesoSwap.pagina);
				if(espacio == NULL)
					{
						// ver la funcionalidadde compactar y rebuscar el espacio libre
					printf("Va 2");
					}
				else
					{
					proceso_ocupado = asignar_espacio_actualizar(pid_proceso_en_curso,cant_pag_requeridas,espacio,list_Libres,list_Ocupados);
					printf("Va 3  \n%d",proceso_ocupado->pid);
					}
				printf("Va 1");
			}else{
				printf("NO hay espacio puto");
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

//funcion que devuelve la catidad de pagina de una lista libre.
int fun_pag(t_espacio_libre* espacio)
{
	return espacio->cant_paginas;
}

// devuelve el total de espacio libre de la lista
int total_Libres(t_list* espacio_Libre)
{

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


t_espacio_ocupado* asignar_espacio_actualizar(int pid, int paginas,t_espacio_libre* espacio, t_list* list_libre,t_list* list_Ocupado)
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

t_espacio_ocupado * list_find_Proceso(t_list* Ocupado, pid_t pid)
{
	bool validar_proceso(t_espacio_ocupado* proceso)
	{
		return(proceso->pid== pid);
	}

	return list_find(Ocupado, (void*) validar_proceso);
}

void Eliminar_De_Archivo_Swap(int comienzo, int cant_paginas)
{
	//abrir de escritura y lectura
	int archivo = open(ruta_Swap,02);
	char* data =mmap((caddr_t)0, cant_paginas*tam_Pagina, PROT_READ|PROT_WRITE ,MAP_SHARED, archivo, comienzo);

	  memset(data,'\0', cant_paginas*tam_Pagina);

	 if (msync(data, cant_paginas*tam_Pagina, MS_SYNC) == -1)
	 {
	  perror("Could not sync the file to disk");
	 }
	 munmap(data, cant_paginas*tam_Pagina);
	 close(archivo);
}
void ordenarLista(t_list* espacio_Libre)
{
	bool comparar_Comienzos(t_espacio_libre* espacio_1, t_espacio_libre* espacio_2)
	{
		return(espacio_1->comienzo < espacio_2->comienzo);
	}

	list_sort(espacio_Libre, (void*) comparar_Comienzos);
}


//revisar: error funcionamiento.
void consolidar_espacio(t_list* espacio_Libre)
{
	ordenarLista(espacio_Libre);
	t_link_element* aux = espacio_Libre->head;
	while(aux!=NULL)
		{
		t_espacio_libre* actual= aux->data;
		int comienzo_Actual= actual->comienzo;
		int cant_paginas_Actual= actual->cant_paginas;
		int termina_Actual = comienzo_Actual+ cant_paginas_Actual;
		if(aux->next!=NULL){
		t_espacio_libre* siguiente= aux->next->data;
		int comienzoSig= siguiente->comienzo;
			if(termina_Actual==comienzoSig)
			{
				actual->cant_paginas= cant_paginas_Actual+siguiente->cant_paginas;

				t_link_element* temp= aux->next;
				aux->next=temp->next;
				free(temp);
				espacio_Libre->elements_count--;
			}
		}
		aux=aux->next;

		}
}

void finalizar_Proceso (t_list* espacio_Libre, t_list* espacio_Ocupado, pid_t pid)
{
	t_espacio_ocupado* proceso= list_find_Proceso(espacio_Ocupado, pid);
	t_espacio_libre* espacio_Proceso=malloc(sizeof(t_espacio_libre));
	espacio_Proceso->comienzo= proceso->comienzo;
	espacio_Proceso->cant_paginas= proceso->cant_paginas;
	list_add(espacio_Libre,espacio_Proceso);
	consolidar_espacio(espacio_Libre);

	bool validar_proceso(t_espacio_ocupado* proceso_ocupa)
		{
			return(proceso_ocupa->pid== pid);
		}

	Eliminar_De_Archivo_Swap(proceso->comienzo, proceso->cant_paginas);
	list_remove_by_condition(espacio_Ocupado,  (void*) validar_proceso);
}

t_prot_cpu_mem* desSerializar(void* buffer, size_t packageSize)
{
	t_prot_cpu_mem*  pedido = malloc(sizeof(t_prot_cpu_mem));

	memcpy(&pedido->tipo_Instruccion,buffer,sizeof(char));
	memcpy(&pedido->pid,buffer + sizeof(char) , sizeof(int));
	memcpy(&pedido->paginas, buffer + sizeof(char) + sizeof(int), sizeof(int));


	return pedido;

}

void responderPedido(int memSocket, t_prot_cpu_mem* pedido,t_list* list_Libres,t_list* list_Ocupados)
{

	t_tipoPedidos tipoPedido  = pedido->tipo_Instruccion;

	switch (tipoPedido)
	{

		case    INICIAR: iniciarProceso(memSocket, pedido,list_Libres,list_Ocupados); break;
		/*case   	 LEER: realizarLectura(memSocket,pedido); break ;
		case  	 FINALIZAR: finalizarProceso(memSocket,pedido); break;
		case  	 ESCRIBIR: finalizarProceso(memSocket,pedido); break;
		*/
		default: tipoDePedidoIncorrecto(memSocket); break;



	}
}

void tipoDePedidoIncorrecto(int memSocket)
{


}




