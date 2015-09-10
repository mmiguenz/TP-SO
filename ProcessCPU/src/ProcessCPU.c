/*
 ============================================================================
 Name        : ProcessCPU.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

/*
 ============================================================================
 Name        :
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>

int main(void) {
    int socket_planificador;
    struct sockaddr_in dire_planificador;
    fd_set rfds;

    char* port_planificador;
    char* ip_planificador;
    port_planificador=malloc(sizeof port_planificador);
    ip_planificador=malloc(sizeof ip_planificador);
    t_config* config_cpu = config_create("config.cfg");



        if (config_cpu != NULL){

        port_planificador= config_get_string_value(config_cpu, "PUERTO_PLANIFICADOR");
        ip_planificador= config_get_string_value(config_cpu, "IP_PLANIFICADOR");
        }



    dire_planificador.sin_family = AF_INET;
    dire_planificador.sin_addr.s_addr = inet_addr(ip_planificador);
	dire_planificador.sin_port = htons(port_planificador);

		int planificador = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(planificador, (void*) &dire_planificador, sizeof(dire_planificador)) != 0) {
			perror("No se pudo conectar");
			return 1;
		}


			char mensaje[1000]="";
			recv(planificador, mensaje,sizeof mensaje,0);
			printf("Recibi mensaje: %s \n", mensaje);


		return 0;
	return EXIT_SUCCESS;
}
