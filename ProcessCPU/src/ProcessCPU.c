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
#include "cliente.h"
#include <pthread.h>
#include "hilito.h"
#include <commons/collections/list.h>
#include <commons/log.h>

int puerto_escucha_planificador;
char* ip_conec_plani;
int puerto_escucha_memoria;
char* ip_conec_memoria;
t_log* logger;

struct param{
	int puerto_escucha_planificador;
	char* ip_conec_plani;
	int puerto_escucha_memoria;
	char* ip_conec_memoria;
	t_log* logger;
};



int main(void) {


	ip_conec_plani= malloc(sizeof ip_conec_plani);
	t_config* config;
	ip_conec_memoria= malloc(sizeof ip_conec_memoria);
    config = config_create("config.cfg");
	if(config != NULL){
		    puerto_escucha_planificador=config_get_int_value(config, "PORT_PLANIFICADOR");
		    ip_conec_plani=config_get_string_value(config,"IP_PLANIFICADOR");
		    puerto_escucha_memoria=config_get_int_value(config, "PORT_MEMORIA");
		    ip_conec_memoria=config_get_string_value(config,"IP_MEMORIA");
		    logger = log_create("log.txt", "CPU",false, LOG_LEVEL_INFO);
	}


	pthread_t hilito;
	struct param param1 = { puerto_escucha_planificador,ip_conec_plani, puerto_escucha_memoria, ip_conec_memoria, logger};
	pthread_create(&hilito, NULL, (void*)conectar,(void*)&param1 );
	pthread_join(hilito, NULL);
	//config_destroy(config);
	free ( ip_conec_memoria);
	free ( ip_conec_plani);
	return EXIT_SUCCESS;
}
