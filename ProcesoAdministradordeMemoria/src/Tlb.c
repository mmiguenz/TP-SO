/*
 * Tlb.c
 *
 *  Created on: 22/10/2015
 *      Author: utnso
 */



#include "Tlb.h"
int ** inicializarTLB(int entradas_TLB);


TLB* t_tlb_crear(t_paramConfigAdmMem* config)
{
	TLB* tlb  = malloc(sizeof(TLB));
	tlb->CacheTLB = inicializarTLB(config->entradas_TLB);

	tlb->sOrdenDeIngresoTLB = queue_create();

return  tlb;
}

void t_tlb_eliminar(TLB* tlb,t_paramConfigAdmMem* config)
{
	int i = 0;
	for(i ; i <config->entradas_TLB;i++)
		free(tlb->CacheTLB[i]);

	free(tlb->CacheTLB);

	queue_destroy(tlb->sOrdenDeIngresoTLB);


}




int ** inicializarTLB(int entradas_TLB)
{
	int ColNumPagina = 0;
	int i;
	/*
		Columnas de la tabla a definir con grupo CASCANUECES  :

	*/
				    int** TLB = malloc(entradas_TLB*sizeof(int*));
				    for (i=0; i<entradas_TLB; ++i)
				    {
				    	TLB[i] = calloc(3, sizeof(int));
				    }
		//cargo valores por defecto -1 para saber que esta vacia:
		for (i = 0; i < entradas_TLB; i++)
		{
			TLB[i][ColNumPagina] = -1;
		}
	return TLB;
}


void t_tlb_limpiar(TLB* tlb, int pid )
{


}

