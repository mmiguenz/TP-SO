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

	 int puerto_escucha_cpu;
	 char* ip_conec_plani;
	 ip_conec_plani= malloc(sizeof ip_conec_plani);
	 t_config* config;

	                        	//puerto_escucha_planif=malloc(sizeof puerto_escucha_planif);
	                        	config = config_create("config.cfg");
	                        	if(config != NULL){
	                        	puerto_escucha_cpu=config_get_int_value(config, "PORT");
	                        	ip_conec_plani=config_get_string_value(config,"IP");}


    int socket_cpu;
    struct sockaddr_in dire_serv;
    fd_set rfds;
    dire_serv.sin_family = AF_INET;
    dire_serv.sin_addr.s_addr = inet_addr(ip_conec_plani);
	dire_serv.sin_port = htons(puerto_escucha_cpu);

		int cliente = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(cliente, (void*) &dire_serv, sizeof(dire_serv)) != 0) {
			perror("No se pudo conectar");
			return 1;
		}


			char mensaje[1000]="";
			recv(cliente, mensaje,sizeof mensaje,0);
			printf("Recibi mensaje: %s \n", mensaje);


		return 0;
	return EXIT_SUCCESS;
}
