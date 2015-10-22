/*
 * EstructurasParaMemoria.h
 *
 *  Created on: 6/10/2015
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_
#include <commons/collections/list.h>
#include <commons/collections/queue.h>



typedef struct{
		char** Memoria;
		int* MemoriaLibre;
	}MEMORIAPRINCIPAL;



	typedef struct{
		int totalDePaginas;
		int ** tablaDePaginas;
		t_list* marcosAsignados;
		t_queue* contenidoDeMarcos;
		int contadorMissTLB;
		int contadorHits;
	}TABLADEPROCESOS;

	char ** inicializarMemoriaPrincipal(int Cant_Marcos, int Tamanio_Marco);
	int ** inicializarTLB(int entradas_TLB);
	int* inicializarMemoriaLibre(int cant_Marcos);
	int Marcoslibres(int cantidadDePaginas, int Cant_Marcos, int* memoriaLibre);
	t_list* asignarMarcos_MemoriaPrincipal(t_list* marcosAsignados, int* memoriaLibre, int MarcosPorProcesos);
	int algoritmo_FirstFit_MEMORIA(int Cant_Marcos, int* memoriaLibre);
	void inicializarProceso(int PID, int totalDePaginas, int* memoriaLibre, int marcosPorProcesos);
	void finalizarProceso(int pid, MEMORIAPRINCIPAL* memoria);
	int ** inicializarTablaDePaginas(int cantPagina);
	void resetearMarco(void * numeroMarco);



#endif /* MEMORIA_H_ */
