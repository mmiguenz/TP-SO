/*
 ============================================================================
 Name        : Cache13-CascaNuts-ProcessPlanificador.c
 Author      : GrupoCascaNuts
 Version     :
 Copyright   : Ante Cualquier copia se le formateara la pc ;)
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

/** Puerto  */
#define PORT       7000


/** Longitud del buffer  */
#define BUFFERSIZE 512

int main(void) {
//	t_log* Planificador;
	//Planificador = log_create("Nuevo_Log", "Panificador", 1, LOG_LEVEL_INFO) ; //Creo el primer archivo de log del proceso planificador
//	log_debug(Planificador, "Primer Log");//Porque no hace nada?
//	log_impl_template(log_info, LOG_LEVEL_INFO);


    int socket_host;
    struct sockaddr_in dire_serv;

    struct timeval tv;      /* Para el timeout del accept */
    socklen_t size_addr = 0;
    int socket_client;
    fd_set rfds;
    dire_serv.sin_family = AF_INET;
    dire_serv.sin_addr.s_addr = inet_addr("10.15.90.166");
	dire_serv.sin_port = htons(8080);
	t_config* config;
config = config_create("Resources/Config.cfg");
printf("Lei mi archivo: %s",config_get_string_value(config, "IP"));

int cliente = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(cliente, (void*) &dire_serv, sizeof(dire_serv)) != 0) {
			perror("No se pudo conectar");
			return 1;
		}

		while (1) {
			char mensaje[1000];

			scanf("%s", mensaje);

			send(cliente, mensaje, strlen(mensaje), 0);
		}

		return 0;

	puts("!!!Planificacion!!"); /* prints !!!Planificacion!! */
	return EXIT_SUCCESS;
}
