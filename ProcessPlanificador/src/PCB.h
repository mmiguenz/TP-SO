/*
 * PCB.h
 *
 *  Created on: 18/9/2015
 *      Author: utnso
 */

#ifndef PCB_H_
#define PCB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/queue.h>


typedef struct {
char* nombreProc;
int estado;
int PID;
int contadorProgram;
}PCB ;


static PCB *pcb_create(char *name, int estado);

static void pcb_destroy(PCB *self);



#endif /* PCB_H_ */
