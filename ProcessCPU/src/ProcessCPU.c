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

int main(void) {

	 int puerto_escucha_planificador;
	 char* ip_conec_plani;
	 ip_conec_plani= malloc(sizeof ip_conec_plani);
	 t_config* config;

	                        	//puerto_escucha_planif=malloc(sizeof puerto_escucha_planif);
	                        	config = config_create("config.cfg");
	                        	if(config != NULL){
	                        	puerto_escucha_planificador=config_get_int_value(config, "PORT_PLANIFICADOR");
	                        	ip_conec_plani=config_get_string_value(config,"IP_PLANIFICADOR");}


    int socket_planificador;
    struct sockaddr_in dire_serv;
    fd_set rfds;
    dire_serv.sin_family = AF_INET;
    dire_serv.sin_addr.s_addr = inet_addr(ip_conec_plani);
	dire_serv.sin_port = htons(puerto_escucha_planificador);

		int planificador = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(planificador, (void*) &dire_serv, sizeof(dire_serv)) != 0) {
			perror("No se pudo conectar");
			return 1;
		}


			char mensaje[1000]="";
			recv(planificador, mensaje,sizeof mensaje,0);
			printf("Recibi mensaje: %s \n", mensaje);



//-----------------------conectandome con memoria y mandando msj--------------


		int puerto_escucha_memoria;
		 char* ip_conec_memoria;
		ip_conec_memoria= malloc(sizeof ip_conec_memoria);
			 //t_config* config;

			                        	//puerto_escucha_planif=malloc(sizeof puerto_escucha_planif);
			                        	//config = config_create("config.cfg");
			                        	//if(config != NULL){
			                        	puerto_escucha_memoria=config_get_int_value(config, "PORT_MEMORIA");
			                        	ip_conec_memoria=config_get_string_value(config,"IP_MEMORIA");



		    struct sockaddr_in dire_memoria;

		    dire_memoria.sin_family = AF_INET;
		    dire_memoria.sin_addr.s_addr = inet_addr(ip_conec_memoria);
			dire_memoria.sin_port = htons(puerto_escucha_memoria);

				int memoria = socket(AF_INET, SOCK_STREAM, 0);
				if (connect(memoria, (void*) &dire_memoria, sizeof(dire_memoria)) != 0) {
					perror("No se pudo conectar");
					return 1;
				}



					send(memoria, mensaje,sizeof mensaje,0);
					printf("Envie mensaje: %s \n", mensaje);


				return 0;


	return EXIT_SUCCESS;
}
