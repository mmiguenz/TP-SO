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




/*struct conexion_cliente_servidor{
	int puerto_escucha_swap;
	char* ip_conec_swap;
	char* puerto_escucha_memoria;
}conexion;*/



 int main(void){
	 char* puerto_escucha_memoria;
	 int puerto_escucha_swap;
	 char* ip_conec_swap;
     t_config* config;
    // t_log* logger;

                        	puerto_escucha_memoria=malloc(sizeof puerto_escucha_memoria);
                        	    	config = config_create("config.cfg");
                        	if(config != NULL){
                        	puerto_escucha_memoria=config_get_string_value(config, "PORT_ESCUCHA");
                        	puerto_escucha_swap=config_get_int_value(config, "PORT_SWAP");
                        	ip_conec_swap=config_get_string_value(config,"IP_SWAP");
                        	//logger = log_create("log.txt", "Administrador de memoria",false, LOG_LEVEL_INFO);

                        	}

                        	//struct conexion_cliente_servidor memoriaG = { puerto_escucha_swap,ip_conec_swap, puerto_escucha_memoria};
	//----------Soy una barra separadora ;)--------------------------------------//
                        	int swap = conectar_cliente(puerto_escucha_swap, ip_conec_swap);

                        	 conectar_servidor(puerto_escucha_memoria, swap);
                        	 return EXIT_SUCCESS;
 }

