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
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include "servidor.h"
#include "Cliente.h"
#include "EstructurasParaMemoria.h"
#include "TADConfig.h"
#include "ProtocsyFuncsRecvMsjs.h"
#include "ProcesoAdministradordeMemoria.h"

#define SI "SI"

typedef struct{
	int idFrame;
	char bitPresencia;
	char bitModificado;
}t_regTablaPaginas;

void crear_e_insertar_TabladePaginas(int, char*, t_dictionary*);
void lecturaMemoria(int);
void escrituraMemoria(int);
void finalizacionProceso(int);

	 t_config* config;
	 t_log* logAdmMem;
	 t_paramConfigAdmMem* configAdmMem;
	 MEMORIAPRINCIPAL memoriaPrincipal;
	 t_dictionary* tablasPags;
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
	 	 	 	 tablasPags = malloc(sizeof(t_dictionary));
	 	 	 	 tablasPags = dictionary_create();

	 	 	 	 int swap = conectar_cliente(configAdmMem->puerto_swap,configAdmMem->ip_swap);
	 	 	 	 //conectar_servidor(configAdmMem->puerto_escucha, swap, memoriaPrincipal.MemoriaLibre,Max_Marcos_Por_Proceso,Cant_Marcos,memoriaPrincipal.Memoria);
	 	 	 	 return EXIT_SUCCESS;
 }

 void procesarPedido(int socketCPU, int socketSwap, char tipoInstruccion){
	 enum {iniciar = 1,leer,escribir,finalizar};

	 switch(tipoInstruccion){

	 	 case iniciar:{
		 	 inicializacionProceso(socketCPU,socketSwap);
	 	 }
	 	 break;

	 	 case leer:{
		 	 lecturaMemoria(socketCPU);
	 	 }
	 	 break;

	 	 case escribir:{
		 	 escrituraMemoria(socketCPU);
	 	 }
	 	 break;

	 	 case finalizar:{
		 	 finalizacionProceso(socketCPU);
	 	 }
	 	 break;
	 }
}

 void inicializacionProceso(int socketCPU, int socketSwap){

	 	 void* buffer = malloc(sizeof(int)*2);
	 	 char* confirmSwap = malloc(sizeof(char));
	 	 char* confirmCPU = malloc(sizeof(char));


	 	 //Recibo la estructura serializada del CPU y la cargo en estructura correspondiente
	 	 t_protoc_inicio_lectura_Proceso* protInic = malloc(sizeof(t_protoc_inicio_lectura_Proceso));;
	 	 recv(socketCPU,protInic->paginas,sizeof(int),0);
	 	 recv(socketCPU,protInic->PID,sizeof(int),0);

	 	 memcpy(buffer,protInic->PID,sizeof(int));
	 	 memcpy(buffer+sizeof(int),protInic->paginas,sizeof(int));
	 	 send(socketSwap,buffer,sizeof(int)*2,0);
	 	 recv(socketSwap,confirmSwap,sizeof(char),0); /* Recibo del Swap la confirmación de asignación de memoria al proceso */
	 	 confirmCPU = confirmSwap;
	 	 send(socketCPU,confirmCPU,sizeof(char),0); /* Envío al CPU la señal de asignación o no de memoria al proceso que se inicia */

	 	 if (*confirmSwap == 1) {
	 	 //Creación de la tabla de páginas del proceso y agregado de la misma a la Lista de Tablas de Páginas
	 		 char* pidConv = malloc(sizeof(char*));
	 		 pidConv = string_itoa(*(protInic->PID));
	 		 crear_e_insertar_TabladePaginas(*(protInic->paginas), pidConv, tablasPags);
	 	 }

	 	 free(protInic);
 };
 void lecturaMemoria(int socket){
 };
 void escrituraMemoria(int socket){
 };
 void finalizacionProceso(int socket){
 };

 void crear_e_insertar_TabladePaginas(int paginas, char* pid, t_dictionary* tablasPagsProcesos) {
	 t_regTablaPaginas** tablaPaginasProceso = calloc(paginas,sizeof(t_regTablaPaginas*));
	 int i;
	 for (i=0; i<paginas; ++i){
	 	 	t_regTablaPaginas* tempTabPags = malloc(sizeof(t_regTablaPaginas));
	 	 	tempTabPags->idFrame = 0;
	 	 	tempTabPags->bitPresencia = 0;
	 	 	tempTabPags->bitModificado = 0;
	 	 	tablaPaginasProceso[i] = tempTabPags;
	 	 	free(tempTabPags);
	 };

	 dictionary_put(tablasPagsProcesos,pid,tablaPaginasProceso);
 }
