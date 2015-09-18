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

void shell(int listener, int skEmisor, int skReceptor, char * buf, int nbytes);

void *get_in_addr(struct sockaddr *sa);

void limpiar (char *cadena);

int tamaniobuf(char cad[]);

int main(void)
{
	 char* puerto_escucha_memoria;
                        	t_config* config;

                        	puerto_escucha_memoria=malloc(sizeof puerto_escucha_memoria);
                        	config = config_create("config.cfg");
                        	if(config != NULL){
                        	puerto_escucha_memoria=config_get_string_value(config, "PORT_ESCUCHA");}



	//----------Soy una barra separadora ;)--------------------------------------//

	fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256]="";    // buffer for client data

    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, puerto_escucha_memoria, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }
        //  "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        break;
    }
    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from %s on "
                            "Cpu %d\n",//Uso el identificador del soket como nombre del soket
                            inet_ntop(remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN),
                            newfd);
                        char mensaje[1000]="";

                        			recv(newfd, mensaje,sizeof mensaje,0);
                        			printf("Recibi mensaje: %s \n", mensaje);

                        			if(true){
                        			 int puerto_escucha_swap;
                        			 char* ip_conec_swap;
                        			 ip_conec_swap= malloc(sizeof ip_conec_swap);
                        				 puerto_escucha_swap=config_get_int_value(config, "PORT_SWAP");
                        				 ip_conec_swap=config_get_string_value(config,"IP_SWAP");
                        			 struct sockaddr_in dire_servi;
                        			 dire_servi.sin_family = AF_INET;
                        			 dire_servi.sin_addr.s_addr = inet_addr(ip_conec_swap);
                        			 dire_servi.sin_port = htons(puerto_escucha_swap);

                        			 int planificador = socket(AF_INET, SOCK_STREAM, 0);
                        			 if (connect(planificador, (void*) &dire_servi, sizeof(dire_servi)) != 0) {
                        			 perror("No se pudo conectar");}
                        			 send(planificador,mensaje,sizeof mensaje,0);
                        			                        				}

                    }
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);

                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                       // we got some data from a client
                       for(j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &master)&& i!=j) {
                                // except the listener and ourselves
                                shell (listener, i, j , buf, nbytes);
                           }
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!

    return 0;
}

void shell(int listener, int skEmisor, int skReceptor, char * buf, int nbytes){

    printf("%s\n", buf);//action);
}





// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void limpiar (char *cadena)
{
  char *p;
  p = strchr (cadena, '\n');
  if (p)
    *p = '\0';
}



int tamaniobuf(char cad[])
{
   int pos = -1;
   int len = strlen( cad);
int i;
   for( i = 0; pos == -1 && i < len; i++){ // si quitas la condición pos == -1
            // te devuelve la última posición encontrada (si es que hay más de 1)
      if(*(cad+i) == '\0')
         pos = i+1;
   }
   return pos;


	/* int puerto_escucha_swap;
	 char* ip_conec_swap;
	 ip_conec_swap= malloc(sizeof ip_conec_swap);
	 t_config* config;

	                        	//puerto_escucha_planif=malloc(sizeof puerto_escucha_planif);
	                        	config = config_create("config");
	                        	if(config != NULL){
	                        	puerto_escucha_swap=config_get_int_value(config, "PORTSWAP");
	                        	ip_conec_swap=config_get_string_value(config,"IPSWAP");}


  int socket_swap;
  struct sockaddr_in dire_serv;
  fd_set rfds;
  dire_serv.sin_family = AF_INET;
  dire_serv.sin_addr.s_addr = inet_addr(ip_conec_swap);
	dire_serv.sin_port = htons(puerto_escucha_swap);

		int swap = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(swap, (void*) &dire_serv, sizeof(dire_serv)) != 0) {
			perror("No se pudo conectar");
			return 1;
		}
			char mensaje[1000]="";
			recv(swap, mensaje,sizeof mensaje,0);
			printf("Recibi mensaje: %s \n", mensaje);*/

}

