
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

int main(void) {


    int socket_host;
    struct sockaddr_in client_addr;

    struct timeval tv;      /* Para el timeout del accept */
    socklen_t size_addr = 0;
    int socket_client;
    fd_set rfds;


    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(8084);

	int servidor = socket(AF_INET, SOCK_STREAM, 0);

	if (bind(servidor, (void*) &client_addr, sizeof(client_addr)) != 0) {
		perror("Falló el bind");
		return 1;
	}

	printf("Estoy escuchando\n");
	listen(servidor, 100);
	close(servidor);


	for(;;);

	return 0;
}
