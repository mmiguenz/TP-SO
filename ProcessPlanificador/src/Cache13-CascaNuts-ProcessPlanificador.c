/*
 ============================================================================
 Name        : Cache13-CascaNuts-ProcessPlanificador.c
 Author      : GrupoCascaNuts
 Version     :
 Copyright   : Ante Cualquier copia se le formateara la pc ;)
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
#include <commons/config.h>
#include <commons/log.h>
#include <regex.h>
#include <sys/ioctl.h>
#include <net/if.h>


/** Puerto  */
#define PORT       7000


/** Longitud del buffer  */
#define BUFFERSIZE 512






// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



int getPosStr(char *dest, char *buf, char sep, int index) {
    char *ret;
    char *ptr;
    int indexCoincidencia = 0, i = 0;
    ret = dest;

    ret[0] = '\0';
    for (ptr = buf; *ptr != '\0'; ptr++) {
        if (indexCoincidencia > index)
            return indexCoincidencia;
        if (indexCoincidencia == index && sep != *ptr) {
            ret[i++] = *ptr;
            ret[i] = '\0';
        }
        if (*ptr == sep)
            indexCoincidencia++;
    }
    return indexCoincidencia;
}



int main(void) {

	t_config* config;
config = config_create("Resources/Config.cfg");
printf("Lei mi archivo: %s",config_get_string_value(config, "IP"));


fd_set master;    // lista de descriptores maestra
fd_set read_fds;  // lista temporal de descriptores para el select()
int fdmax;        // numero maximo de descriptores

int listener;     // socket de escucha
int newfd;        // newly accept()ed socket descriptor
struct sockaddr_storage remoteaddr; // direccion del cliente
socklen_t addrlen;

char buf[256];    // buffer para la informacion del cliente
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
if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
    fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
    exit(1);
}

for(p = ai; p != NULL; p = p->ai_next) {
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0) {
        continue;
    }

    // lose the pesky "address already in use" error message
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
//..
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
                        "socket %d\n",
                        inet_ntop(remoteaddr.ss_family,
                            get_in_addr((struct sockaddr*)&remoteaddr),
                            remoteIP, INET6_ADDRSTRLEN),
                        newfd);
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
                        if (FD_ISSET(j, &master)) {
                            // except the listener and ourselves
                            shell (listener, i, j , buf, nbytes);
                        }
                    }
                }
            } // END handle data from client
        } // END got new incoming connection
    } // END looping through file descriptors
} // END for(;;)--and you thought it would never end!


	return EXIT_SUCCESS;
}




void shell(int listener, int skEmisor, int skReceptor, char * buf, int nbytes){

    char** comando = (char**)malloc(sizeof(char) * 1024 * 255);

    char action [255] ;
    char cpu [255] ;
    char msg [255] ;


    getPosStr(action,buf, ',',0);

    printf("%s\n", action);



    if (strncmp (action, "run", 3) == 0){
        getPosStr(cpu,buf, ',',1);

        printf("%s\n", cpu);


        int skCpu = atoi(cpu);

        if (skCpu == skReceptor){
        getPosStr(msg,buf, ',',2);
        printf("%s\n", msg);

            if (send(skCpu, msg, nbytes, 0) == -1) {
                perror("send");
            }
        }

    }


    if (strncmp (buf, "ls", 2) == 0){
        printf("planificador\n");
    }


}



