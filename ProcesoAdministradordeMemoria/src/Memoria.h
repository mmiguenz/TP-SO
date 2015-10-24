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
#include <commons/collections/dictionary.h>



typedef struct{
		char** Memoria;
		char** MemoriaLibre;
		int cantMarcos;
	}MEMORIAPRINCIPAL;

	typedef struct{
		int idFrame;
		char bitPresencia;
		char bitModificado;
		int horaIngreso;
	}t_regPagina;

	typedef struct{
		t_regPagina** Pagina;
		int pid;
		int cantTotalPaginas;
	}t_tablaDePaginas;


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
	char** inicializarMemoriaLibre(int cant_Marcos);
	int Marcoslibres(int cantidadDePaginas, int Cant_Marcos, int* memoriaLibre);
	t_list* asignarMarcos_MemoriaPrincipal(t_list* marcosAsignados, int* memoriaLibre, int MarcosPorProcesos);
	void inicializarProceso(int PID, int totalDePaginas, int* memoriaLibre, int marcosPorProcesos);
	void finalizarProceso(int pid, MEMORIAPRINCIPAL* memoria);
	void resetearMarco(void * numeroMarco);
	int buscarPaginaenMemoria(int, int,t_dictionary*);
	int insertarContenidoenMP(int,char*,MEMORIAPRINCIPAL, t_tablaDePaginas*);
	int reemplazarPaginaFIFO (int,char*, MEMORIAPRINCIPAL, t_tablaDePaginas*);
	void enviarDatosPorModifASwap(int,char*,int,int);
	void actualizarTablaPagsProceso(int,int,t_tablaDePaginas*);

#endif /* MEMORIA_H_ */