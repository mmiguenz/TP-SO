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



int main(void)
{
	 char* puerto_escucha_memoria;
                        	t_config* config;

                        	puerto_escucha_memoria=malloc(sizeof puerto_escucha_memoria);
                        	config = config_create("config.cfg");
                        	if(config != NULL){
                        	puerto_escucha_memoria=config_get_string_value(config, "PORT_ESCUCHA");}



	//----------Soy una barra separadora ;)--------------------------------------//

                        	 conectar_servidor( puerto_escucha_memoria);



}

