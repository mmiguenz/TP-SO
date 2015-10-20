/*
 ============================================================================
 Name        : ProcesoAdministradordeMemoria.c
 Author      : Gisell Lara
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#include <string.h>
#include <regex.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <commons/config.h>
#include "servidor.h"
#include "Cliente.h"
#include "EstructurasParaMemoria.h"
#include "TADConfig.h"

#define SI "SI"

void inicializacionProceso(int);
void lecturaMemoria(int);
void escrituraMemoria(int);
void finalizacionProceso(int);

	 t_config* config;
	 t_log* logAdmMem;
	 t_paramConfigAdmMem* configAdmMem;
	 MEMORIAPRINCIPAL memoriaPrincipal;
	 TLB tlb;

 int main(void){

	 	 	 	 /* Se levanta el archivo de configuración y se crea log del Administrador de Memoria*/
	 	 	 	 configAdmMem = establecerConfigMemoria();
	 	 	 	 logAdmMem = log_create("log.txt", "Administrador de memoria",false, LOG_LEVEL_INFO);

	 	 	 	 /* Inicialización de espacio de memoria, array indicador de memoria libre y TLB */

	 	 	 	 memoriaPrincipal.Memoria=inicializarMemoriaPrincipal(configAdmMem->cantidad_marcos,configAdmMem->tamanio_marco);
	 	 	 	 memoriaPrincipal.MemoriaLibre=inicializarMemoriaLibre(configAdmMem->cantidad_marcos);

	 	 	 	 if (strcmp(configAdmMem->tlb_habilitada,SI)) {
	 	 	 	 tlb.CacheTLB=inicializarTLB(configAdmMem->entradas_TLB);
	 	 	 	 }

	 	 	 	 int swap = conectar_cliente(configAdmMem->puerto_swap,configAdmMem->ip_swap);
	 	 	 	 //conectar_servidor(configAdmMem->puerto_escucha, swap, memoriaPrincipal.MemoriaLibre,Max_Marcos_Por_Proceso,Cant_Marcos,memoriaPrincipal.Memoria);
	 	 	 	 return EXIT_SUCCESS;
 }
 void procesarPedido(int socket, char tipoInstruccion){
	 enum {iniciar = 1,leer,escribir,finalizar};

	 switch(tipoInstruccion){

	 	 case iniciar:{
		 	 inicializacionProceso(socket);
	 	 }
	 	 break;

	 	 case leer:{
		 	 lecturaMemoria(socket);
	 	 }
	 	 break;

	 	 case escribir:{
		 	 escrituraMemoria(socket);
	 	 }
	 	 break;

	 	 case finalizar:{
		 	 finalizacionProceso(socket);
	 	 }
	 	 break;
	 }
}

 void inicializacionProceso(int socket){
 };
 void lecturaMemoria(int socket){
 };
 void escrituraMemoria(int socket){
 };
 void finalizacionProceso(int socket){
 };
