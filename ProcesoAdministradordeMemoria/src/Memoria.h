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
#include "TADConfig.h"


typedef struct{
		char** Memoria;
		char* MemoriaLibre;
		int cantMarcos;
		int tamanioMarco;
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

	char ** inicializarMemoriaPrincipal(MEMORIAPRINCIPAL* memoriaP ,int Cant_Marcos, int Tamanio_Marco);
	char* inicializarMemoriaLibre(int cant_Marcos);
	int Marcoslibres(int cantidadDePaginas, int Cant_Marcos, int* memoriaLibre);
	void finalizarProceso(MEMORIAPRINCIPAL* memoria,t_tablaDePaginas* tablaDePaginas);
	int buscarPaginaenMemoria(int, int,t_dictionary*);
	void insertarPaginaenMP(char*,MEMORIAPRINCIPAL*,int*);
	int reemplazarPaginaFIFO (int,char*, MEMORIAPRINCIPAL*, t_tablaDePaginas*,int*);
	void enviarDatosPorModifASwap(int,char*,int,int,int);
	void actualizarTablaPaginas(int,int,int,t_tablaDePaginas*);
	void inicializarMarco(MEMORIAPRINCIPAL*, char* marco);
	void t_memoria_crear(MEMORIAPRINCIPAL* ,t_paramConfigAdmMem* config);
	int buscarFrameLibre(MEMORIAPRINCIPAL* memoria);
	int marcosUtilizadosProceso(t_tablaDePaginas* tablaPagsProceso);
	void mem_Flush(MEMORIAPRINCIPAL* memoria,t_dictionary* tablaDePaginas);


#endif /* MEMORIA_H_ */
