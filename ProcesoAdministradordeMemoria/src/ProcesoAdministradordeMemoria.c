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




//Variables de Archivo de Configuracion:
	int Retardo;
	int Cant_Marcos;
	int Tamanio_Marco;
	int Max_Marcos_Por_Proceso;

	char* TLB_Habilitada;
	int Entradas_TLB;
	char* Tipo_Asignacion;
	char* Algoritmo_De_Memoria;
	char* puerto_escucha_memoria;
	int puerto_escucha_swap;
	char* ip_conec_swap;

	 t_config* config;
	 t_log* logs;
	 MEMORIAPRINCIPAL memoriaPrincipal;
	 TLB tlb;

 int main(void){



                        	puerto_escucha_memoria=malloc(sizeof puerto_escucha_memoria);
                        	config = config_create("config.cfg");
                        	if(config != NULL){

                        	puerto_escucha_memoria=config_get_string_value(config, "PORT_ESCUCHA");
                        	puerto_escucha_swap=config_get_int_value(config, "PORT_SWAP");
                        	ip_conec_swap=config_get_string_value(config,"IP_SWAP");
                        	Retardo = config_get_int_value(config,"RETARDO");
                        	Cant_Marcos =  config_get_int_value(config,"CANTIDAD_MARCOS");
                        	Tamanio_Marco =  config_get_int_value(config,"TAMANIO_MARCO");
                        	Max_Marcos_Por_Proceso =  config_get_int_value(config,"MAX_MARCOS_POR_PROCESO");
                        	TLB_Habilitada = config_get_string_value(config,"TLB_HABILITADA");
                        	Entradas_TLB = config_get_int_value(config,"ENTRADAS_TLB");
                        	logs= log_create("log.txt", "Administrador de memoria",false, LOG_LEVEL_INFO);
                        	}

                        	int swap = conectar_cliente(puerto_escucha_swap, ip_conec_swap);

                        	memoriaPrincipal.Memoria=inicializarMemoriaPrincipal(Cant_Marcos,Tamanio_Marco);
                        	memoriaPrincipal.MemoriaLibre=inicializarMemoriaLibre(Cant_Marcos);
                        	tlb.CacheTLB=inicializarTLB(Entradas_TLB);
                        	 conectar_servidor(puerto_escucha_memoria, swap, memoriaPrincipal.MemoriaLibre,Max_Marcos_Por_Proceso,Cant_Marcos,memoriaPrincipal.Memoria);
                        	 return EXIT_SUCCESS;
 }

