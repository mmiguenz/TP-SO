/*
 * Tlb.c
 *
 *  Created on: 22/10/2015
 *      Author: utnso
 */


#include <stdlib.h>
#include "Tlb.h"
t_regTLB** inicializarTLB(int entradasTLB);


TLB* t_tlb_crear(t_paramConfigAdmMem* config){

		TLB* tlb = malloc(sizeof(TLB));
		tlb->CacheTLB = inicializarTLB(config->entradas_TLB);
		tlb->sOrdenDeIngresoTLB = queue_create();
		tlb->entradasTLB = config->entradas_TLB;

		return tlb;
	//tlb->CacheTLB = inicializarTLB(config->entradas_TLB);
	//tlb->sOrdenDeIngresoTLB = queue_create();

}

void t_tlb_eliminar(TLB* tlb,t_paramConfigAdmMem* config)
{
	int i;
	for(i=0 ; i <config->entradas_TLB;i++)
		free(tlb->CacheTLB[i]);

	free(tlb->CacheTLB);

	queue_destroy(tlb->sOrdenDeIngresoTLB);


}




t_regTLB** inicializarTLB(int entradasTLB){

	int i;
	t_regTLB ** regsTLBtemp = calloc(entradasTLB,sizeof(t_regTLB));
	for (i=0;i<entradasTLB;i++){
		regsTLBtemp[i]->frame = -1;
		regsTLBtemp[i]->pagina = -1;
		regsTLBtemp[i]->pid = -1;
	}
	return regsTLBtemp;

	//int ColNumPagina = 0;
	//int i;


	/*
		Columnas de la tabla a definir con grupo CASCANUECES  :

	*/

				//    int** TLB = malloc(entradas_TLB*sizeof(int*));
				//    for (i=0; i<entradas_TLB; ++i)
				//    {
				 //   	TLB[i] = calloc(3, sizeof(int));
				 //   }
		//cargo valores por defecto -1 para saber que esta vacia:
	//	for (i = 0; i < entradas_TLB; i++)
	//	{
	//		TLB[i][ColNumPagina] = -1;
	//	}
	// return TLB;
}


void t_tlb_limpiar(TLB* tlb, int pid )
{


}

int buscarPaginaTLB(TLB* tlb,int pid, int pagina){
	int i;
	int frame;
	frame = -1;
	t_regTLB** regTLBtemp = malloc(tlb->entradasTLB*sizeof(t_regTLB));
	regTLBtemp = tlb->CacheTLB;
	for (i=0; i < tlb->entradasTLB;i++){
		if (regTLBtemp[i]->pid == pid && regTLBtemp[i]->pagina == pagina){
			frame = regTLBtemp[i]->frame;
			i = tlb->entradasTLB;
		}
	}
	return frame;
}

void agregarRegistroTLB(TLB* tlb,int pid, int pagina, int frame){

}