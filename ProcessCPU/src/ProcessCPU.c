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
#include <cliente.h>
#include <pthread.h>
#include <hilito.h>

int main(void) {


	int puerto_escucha_planificador;
	char* ip_conec_plani;
	ip_conec_plani= malloc(sizeof ip_conec_plani);
	 t_config* config;

	 int puerto_escucha_memoria;
	 char* ip_conec_memoria;
	 ip_conec_memoria= malloc(sizeof ip_conec_memoria);


	                        	config = config_create("config.cfg");
	                        	if(config != NULL){
	                        	puerto_escucha_planificador=config_get_int_value(config, "PORT_PLANIFICADOR");
	                        	ip_conec_plani=config_get_string_value(config,"IP_PLANIFICADOR");
	                        	puerto_escucha_memoria=config_get_int_value(config, "PORT_MEMORIA");
	                        	ip_conec_memoria=config_get_string_value(config,"IP_MEMORIA");}



   int planificador = conectar_cliente(puerto_escucha_planificador, ip_conec_plani);

   int memoria = conectar_cliente(puerto_escucha_memoria, ip_conec_memoria);

   char* tamanioPath = recibirMensaje( planificador);
   char* path = recibirMensaje( planificador);

   pthread_t t1;
   pthread_create(&t1, NULL, abrirmCod, NULL);

   	pthread_join(t1, NULL);

   //enviarMesaje(memoria, mensaje);


	return EXIT_SUCCESS;
}
