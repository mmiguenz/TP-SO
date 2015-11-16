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
	int ** inicializarTLB(int entradas_TLB);
	char* inicializarMemoriaLibre(int cant_Marcos);
	int Marcoslibres(int cantidadDePaginas, int Cant_Marcos, int* memoriaLibre);
	t_list* asignarMarcos_MemoriaPrincipal(t_list* marcosAsignados, int* memoriaLibre, int MarcosPorProcesos);
	void inicializarProceso(int PID, int totalDePaginas, int* memoriaLibre, int marcosPorProcesos);
	void finalizarProceso(MEMORIAPRINCIPAL* memoria,t_tablaDePaginas* tablaDePaginas);
	void resetearMarco(void * numeroMarco);
	int buscarPaginaenMemoria(int, int,t_dictionary*);
	void insertarPaginaenMP(char*,MEMORIAPRINCIPAL*,int*);
	int reemplazarPaginaFIFO (int,char*, MEMORIAPRINCIPAL*, t_tablaDePaginas*,int*);
	void enviarDatosPorModifASwap(int,char*,int,int);
	void actualizarTablaPaginas(int,int,t_tablaDePaginas*);
	void inicializarMarco(MEMORIAPRINCIPAL*, char* marco);
	void t_memoria_crear(MEMORIAPRINCIPAL* ,t_paramConfigAdmMem* config);
	int buscarFrameLibre(MEMORIAPRINCIPAL* memoria);
	int marcosUtilizadosProceso(t_tablaDePaginas* tablaPagsProceso);


	/*Recorro el vector de paginas. Contando cuales tienen el bit de presencia en 1 .
	 * Si la cantidad es mayor o igual a maximo de marcos por proceso, significa que no hay frame libre: retorno 0.
	 * Caso Contrario, retorno 1.
	 * Podria darse un caso particular, y es que en mi tabla de paginas, mi proceso no tenga ocupados su maximo de
	 *  frames, pero de todas formas, la memoria podria estar totalmente usada. Es decir no tendria frame para asignar.
	 * Como los reemplazos son locales, no puedo desalojar un frame de otro proceso. Para esta situacion retorno -1,
	 * ya que es un error. Memoria llena.
	 * . */
	int t_hayFrameLibre(MEMORIAPRINCIPAL* memoriaP,t_tablaDePaginas* tablaDePaginasDelProceso,int maximoDeMarcos);
	int  t_cargarContenido(MEMORIAPRINCIPAL*,char* contenido);

#endif /* MEMORIA_H_ */
