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
#include "hilito.h"

int main(void) {



   pthread_t t1;
   pthread_create(&t1, NULL, abrirmCod, NULL);

   	pthread_join(t1, NULL);



	return EXIT_SUCCESS;
}
