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
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>





/** Longitud del buffer  */
#define BUFFERSIZE 512

int main(void) {
//	t_log* Planificador;
	//Planificador = log_create("Nuevo_Log", "Panificador", 1, LOG_LEVEL_INFO) ; //Creo el primer archivo de log del proceso planificador
//	log_debug(Planificador, "Primer Log");//Porque no hace nada?
//	log_impl_template(log_info, LOG_LEVEL_INFO);



    struct sockaddr_in dire_memoria;
    struct sockaddr_in dire_planificador;

    struct timeval tv;      /* Para el timeout del accept */
    socklen_t size_addr = 0;

    char* port_planificador;
    char* port_memoria; //Puerto de escucha
    char* ip_planificador;
    char* ip_memoria;

    t_config* config_cpu = config_create("Resources/config.cfg");

    port_planificador= config_get_string_value(config_cpu, "PUERTO_PLANIFICADOR");
    ip_planificador= config_get_string_value(config_cpu, "IP_PLANIFICADOR");

    port_memoria= config_get_string_value(config_cpu, "PUERTO_MEMORIA");
    ip_memoria= config_get_string_value(config_cpu, "IP_MEMORIA");


    fd_set rfds;
    dire_planificador.sin_family = AF_INET;
    dire_planificador.sin_addr.s_addr = inet_addr(ip_planificador);
	dire_planificador.sin_port = htons(port_planificador);

	fd_set rfds2;
	dire_memoria.sin_family = AF_INET;
	dire_memoria.sin_addr.s_addr = inet_addr(ip_memoria);
	dire_memoria.sin_port = htons(port_memoria);

		int socket_planificador = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(socket_planificador, (void*) &dire_planificador, sizeof(dire_planificador)) != 0) {
			perror("No se pudo conectar");
			return 1;
		}

		int socket_memoria = socket(AF_INET, SOCK_STREAM, 0);
			if (connect(socket_planificador, (void*) &dire_memoria, sizeof(dire_memoria)) != 0) {
				perror("No se pudo conectar");
				return 1;
			}


			char mensaje[1000]="";
			recv(socket_planificador, mensaje,sizeof mensaje,0);
			printf("Recibi mensaje: %s \n", mensaje);
			send(socket_memoria,mensaje,sizeof mensaje,0);


		return 0;
	return EXIT_SUCCESS;
}
