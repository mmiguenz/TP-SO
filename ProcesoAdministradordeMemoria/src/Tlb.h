/*
 * Tlb.h
 *
 *  Created on: 22/10/2015
 *      Author: utnso
 */

#ifndef TLB_H_
#define TLB_H_
#include "TADConfig.h"
#include <commons/collections/queue.h>


	typedef struct{
		int frame;
		int pid;
		int pagina;
	}t_regTLB;

	typedef struct{
			t_regTLB** CacheTLB;
			t_queue* sOrdenDeIngresoTLB;
			int entradasTLB;
	}TLB;

TLB* t_tlb_crear(t_paramConfigAdmMem*);
void t_tlb_eliminar(TLB*,t_paramConfigAdmMem*);
void t_tlb_limpiar(TLB*, int);
//t_regTLB** inicializarTLB(int entradasTLB);
int buscarPaginaTLB(TLB*,int, int);
void agregarRegistroTLB(TLB*,int,int, int);

#endif /* TLB_H_ */



