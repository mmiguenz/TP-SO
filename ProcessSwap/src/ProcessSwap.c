/*
 ============================================================================
 Name        : ProcessSwap.c
 Author      : 
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
#include <regex.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <commons/config.h>
#include "LibSwap.h"



int main(void)
{
	//Espacio para la configuracion del entorno---------------------------<<

	//variables de configuracion.
	char* puerto_escucha_swap;
	char* nombre_swap;
	int cant_pag;
	int tam_pag;
	int retardo_swat;
	int retardo_compactacion;
	t_config* config;


	puerto_escucha_swap=malloc(sizeof puerto_escucha_swap);

	config = config_create("config.cfg");
	if(config != NULL){
		//setea las variables de configuracion.
		puerto_escucha_swap=config_get_string_value(config, "PORT_SWAP");
		nombre_swap=config_get_string_value(config, "NOMBRE_SWAT");
		cant_pag=config_get_int_value(config, "CANTIDAD_PAGINAS");
		tam_pag=config_get_int_value(config, "TAMANIO_PAGINA");
		retardo_swat=config_get_int_value(config, "RETARDO_SWAT");
		retardo_compactacion=config_get_int_value(config, "RETARDO_COMPACTACION");
	}else{
		perror("Error al crear Archivo de configuracion");
		return EXIT_FAILURE;
	}

	//Obtiene el path del archivo creado.
	char* pathArchivoSwap=crearArchivoSwap(nombre_swap,tam_pag,cant_pag);

	t_list* listaEspaciosLibres = crear_ListaLibre(cant_pag);


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
    if ((rv = getaddrinfo(NULL, puerto_escucha_swap, &hints, &ai)) != 0) {
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
                        printf("Recibi mensaje: %s  del administrador de memoria.\n", mensaje);
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
