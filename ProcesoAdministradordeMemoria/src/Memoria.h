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
		char bitUtilizado;
		int horaIngreso;
		int horaUtilizacion;
	}t_regPagina;

	typedef struct{
		t_regPagina** Pagina;
		int pid;
		int cantTotalPaginas;
		int** vectClockModif;
		int posicClockModif;
		int contadorPaginasAcc;
		int contadorPF;
	}t_tablaDePaginas;

	char ** inicializarMemoriaPrincipal(MEMORIAPRINCIPAL* memoriaP ,int Cant_Marcos, int Tamanio_Marco);
	char* inicializarMemoriaLibre(int cant_Marcos);
	int Marcoslibres(int cantidadDePaginas, int Cant_Marcos, int* memoriaLibre);
	void finalizarProceso(MEMORIAPRINCIPAL* memoria,t_tablaDePaginas* tablaDePaginas);
	int buscarPaginaenMemoria(int, int,t_dictionary*);
	void insertarPaginaenMP(char*,MEMORIAPRINCIPAL*,int*);
	int reemplazarPagina(t_paramConfigAdmMem*,int, MEMORIAPRINCIPAL*, t_tablaDePaginas*,int*,int);
	void reemplazarPaginaFIFO(t_tablaDePaginas* tablaPagsProceso,int* nroPagAReemp);
	void reemplazarPaginaLRU(t_tablaDePaginas* tablaPagsProceso,int* nroPagAReemp);
	void reemplazarPaginaClockModif(t_tablaDePaginas* tablaPagsProceso,int* nroPagAReemp,int maxMarcos,int paginaSolicit);
	void enviarDatosPorModifASwap(int,char*,int,int,int);
	void actualizarTablaPaginas(int,int,int,t_tablaDePaginas*);
	void inicializarMarco(MEMORIAPRINCIPAL*, char* marco);
	MEMORIAPRINCIPAL*  t_memoria_crear(t_paramConfigAdmMem* config);
	int buscarFrameLibre(MEMORIAPRINCIPAL* memoria);
	int marcosUtilizadosProceso(t_tablaDePaginas* tablaPagsProceso);
	void mem_Flush(MEMORIAPRINCIPAL* memoria,t_dictionary* tablaDePaginas);
	void actualizarUtilizyModifPag(int instruccion, t_tablaDePaginas* tablaPagsProceso, int pagina);
	void actualizarVectorClockModif(t_paramConfigAdmMem* configAdmMem,t_tablaDePaginas* tablaPagsProceso,int pagina);

#endif /* MEMORIA_H_ */
