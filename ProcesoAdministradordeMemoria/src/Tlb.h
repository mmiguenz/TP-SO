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
#include "LoggingAdmMem.h"


	typedef struct{
		int frame;
		int pid;
		int pagina;
	}t_regTLB;

	typedef struct{
			t_regTLB** CacheTLB;
			t_queue* sOrdenDeIngresoTLB;
			int nroEntradasTLB;
	}TLB;

TLB* t_tlb_crear(t_paramConfigAdmMem*);
void t_tlb_eliminar(TLB*,t_paramConfigAdmMem*);
void tlb_Flush(TLB*);
char t_tlb_estaLibre(t_regTLB* cacheTLB); // recibe un registro tlb e indica si esta libre
//t_regTLB** inicializarTLB(int entradasTLB);
int buscarPaginaTLB(TLB* tlb,int pid, int pagina,int* entradaTLB);

t_tempLogueo* agregar_reemplazarRegistroTLB(TLB* tlb,int pid, int pagina, int frame);

#endif /* TLB_H_ */



