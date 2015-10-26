/*
 * hilito.h
 *
 *  Created on: 19/9/2015
 *      Author: utnso
 */

#ifndef HILITO_H_
#define HILITO_H_


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <commons/string.h>
#include <assert.h>
#include <commons/log.h>
#include <semaphore.h>

typedef struct  {
	int msgtype;
	int payload_size;
}t_msgHeader;

typedef struct  {
	int contadorDePrograma;
	int tiempo;
	int pid;
	int porcentaje;
}PCB_PARCIAL;

typedef struct {
char* nombreProc;
int estado;
int PID;
int contadorProgram; //si es -1 no abrimos, ejecutamos solo finalizar
char* path;
int cpu_asignada;
int quantum; // si el quantum es -1 la planificacion es fifi, sino es round robin
}PCB ;

int porcentajeDeUso(int diff, int instrucciones, int retardo);

int ubicarPunta(char cadena [1500], PCB* PcbAux);

int procesarCadena(char* cadena, int memoria, int planificador, t_log* logger,PCB* PcbAux, int retardo);

int recolectar_instruccion(char* cadena,char comando[15],int punta);

int procesar_instruccion(char* cadena,char comando[15],int punta,char pagina[3], int memoria, int planificador,t_log* logger, PCB* PcbAux, int retardo,char texto[20],time_t comienzo);

int recolectar_pagina(char* cadena, int punta, char pagina[3]);

int identificar_instruccion(char comando[15]);

int recolectar_paginaEscribir(char cadena[1500], int punta, char pagina[3]);

int recolectar_Texto(char cadena[1500], int punta, char texto[15]);

int procesarCadenaConQuantum(int quantum , char cadena[1500], int memoria, int planificador,t_log* logger, PCB* PcbAux, int retardo);

void sentenciaFinalizar(int memoria, int planificador,t_log* logger, PCB* PcbAux, int retardo);

void abrir(PCB* PcbAux, char buffer[1500]);

void* conectar();


#endif /* HILITO_H_ */



