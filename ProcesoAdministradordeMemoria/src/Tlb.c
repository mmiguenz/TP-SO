/*
 * Tlb.c
 *
 *  Created on: 22/10/2015
 *      Author: utnso
 */


#include <stdlib.h>
#include <stdio.h>
#include "Tlb.h"
#include "LoggingAdmMem.h"
t_regTLB** inicializarTLB(int entradasTLB);


TLB* t_tlb_crear(t_paramConfigAdmMem* config){

		TLB* tlb = malloc(sizeof(TLB));
		tlb->CacheTLB = inicializarTLB(config->entradas_TLB);
		tlb->sOrdenDeIngresoTLB = queue_create();
		tlb->nroEntradasTLB = config->entradas_TLB;

		return tlb;

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
	t_regTLB ** regsTLB = malloc(sizeof(t_regTLB)*(entradasTLB));

	for (i=0;i<entradasTLB;i++){
		t_regTLB* registroTLBTemp = malloc(sizeof(t_regTLB));
		registroTLBTemp->frame = -1;
		registroTLBTemp->pagina = -1;
		registroTLBTemp->pid = -1;
		regsTLB[i]= registroTLBTemp;
	}

	return regsTLB;

}


void tlb_Flush(TLB* tlb)
{
	int i ;

	for (i = 0; i< tlb->nroEntradasTLB ; i++)
	{
		t_regTLB* cacheTlb = tlb->CacheTLB[i];

		cacheTlb->pid = -1;
		cacheTlb->frame = -1;
		cacheTlb->pagina = -1;

		}

return;
}

int buscarPaginaTLB(TLB* tlb,int pid, int pagina, int* entradaTLB){
	int i;
	int frame;
	frame = -1;
	for (i=0; i < tlb->nroEntradasTLB;i++){
		if (tlb->CacheTLB[i]->pid == pid && tlb->CacheTLB[i]->pagina == pagina){
			frame = tlb->CacheTLB[i]->frame;
			*entradaTLB = i;
			i = tlb->nroEntradasTLB;
		}
	}
	return frame;
}

t_tempLogueo* agregar_reemplazarRegistroTLB(TLB* tlb,int pid, int pagina, int frame){
	int i=0;
	int* posCacheTLB = malloc(sizeof(int));
	t_tempLogueo* tempLog;

	if (queue_size(tlb->sOrdenDeIngresoTLB)< tlb->nroEntradasTLB){

		while (tlb->CacheTLB[i]->pid != -1){
			 i++;
		 }
		 tlb->CacheTLB[i]->pid = pid;
		 tlb->CacheTLB[i]->pagina = pagina;
		 tlb->CacheTLB[i]->frame = frame;
		 *posCacheTLB = i;
		 queue_push(tlb->sOrdenDeIngresoTLB,posCacheTLB);
		 tempLog = cargaDatosAccesoTLB(pid,pagina,frame,*posCacheTLB);
		 tempLog->regSaliente = 0;
		 tempLog->hit = false;
		 return tempLog;
		}

	else{
		 posCacheTLB = queue_peek(tlb->sOrdenDeIngresoTLB);
		 tempLog = cargaDatosAccesoTLB(tlb->CacheTLB[*posCacheTLB]->pid,tlb->CacheTLB[*posCacheTLB]->pagina,tlb->CacheTLB[*posCacheTLB]->frame,*posCacheTLB);
		 tempLog->regSaliente = true;
		 tempLog->hit = false;
		 tlb->CacheTLB[*posCacheTLB]->pid = pid;
		 tlb->CacheTLB[*posCacheTLB]->pagina = pagina;
		 tlb->CacheTLB[*posCacheTLB]->frame = frame;
		 queue_pop(tlb->sOrdenDeIngresoTLB);
		 queue_push(tlb->sOrdenDeIngresoTLB,posCacheTLB);
		 return tempLog;
	}


}

void borrarRegistroTLBPagReemp(TLB* tlb, int* paginaReemp,int pid){
	int i;
	int posicAElim = 0;
	for (i = 0; i < tlb->nroEntradasTLB; ++i) {
		if(tlb->CacheTLB[i]->pid == pid && tlb->CacheTLB[i]->pagina == *paginaReemp){
			tlb->CacheTLB[i]->pid = -1;
			tlb->CacheTLB[i]->pagina = -1;
			tlb->CacheTLB[i]->frame = -1;
			posicAElim = i;
		}
	}

	if (posicAElim != 0){

		bool buscarEntradaAEliminar(int* entradaTLB)
		{
			return *entradaTLB==posicAElim;
		}

		int* posContElim = list_remove_by_condition(tlb->sOrdenDeIngresoTLB->elements,(void*)buscarEntradaAEliminar);

		printf("Se elimino de la cola de la TLB la entrada: %i",*posContElim);
	}
}


char t_tlb_estaLibre(t_regTLB* cacheTLB)
{
	return (cacheTLB->frame == -1
			&& cacheTLB->pagina==-1
			&& cacheTLB->pid == -1);


}
