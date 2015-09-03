/*
 ============================================================================
 Name        : Cache13-CascaNuts-ProcessPlanificador.c
 Author      : GrupoCascaNuts
 Version     :
 Copyright   : Ante Cualquier copia se le formateara la pc ;)
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <commons/log.h>

/** Puerto  */
#define PORT       7000


/** Longitud del buffer  */
#define BUFFERSIZE 512

int main(void) {
	t_log* Planificador;
	Planificador = log_create("Nuevo", "Panificador", 1, LOG_LEVEL_INFO) ; //Creo el primer archivo de log del proceso planificador
	log_trace(Planificador, "Primer Log");//Porque no hace nada?
	puts("!!!Planificacion!!"); /* prints !!!Planificacion!! */
	return EXIT_SUCCESS;
}
