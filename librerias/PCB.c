/*
 * PCB.c
 *
 *  Created on: 18/9/2015
 *      Author: utnso
 */
#include "PCB.h"

static PCB *pcb_create(char *name, int estado){
	PCB *new = malloc( sizeof(PCB) );
	new->nombreProc = name;
	new->PID = 0;
	new->estado=0;
	new->contadorProgram=0;
	return new;
}


static void pcb_destroy(PCB *self){
	free(self->nombreProc);
	free(self);
}

