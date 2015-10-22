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
		int** CacheTLB;
		t_queue* sOrdenDeIngresoTLB;
	} TLB;


#endif /* TLB_H_ */


TLB* t_tlb_crear(t_paramConfigAdmMem* );
void t_tbl_eliminar(TLB*,t_paramConfigAdmMem*);
void t_tlb_limpiar(TLB*, int pid );
