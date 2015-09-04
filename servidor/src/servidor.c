
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

//**********Soy una barra inicial********************************************//
//**********Estructuras para el socket**************************************//


    struct sockaddr_in servidor_addr;



    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_addr.s_addr = INADDR_ANY;
    servidor_addr.sin_port = htons(8080);

 //**********Soy una barra separadora :P**************************************//
//**********Creo la coneccion del soket**************************************//

	int servidor = socket(AF_INET, SOCK_STREAM, 0);

	int activado = 1;

	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if (bind(servidor, (void*) &servidor_addr, sizeof(servidor_addr)) != 0) {
		perror("Falló el bind");
		return 1;
	}

	printf("Estoy escuchando\n");
	listen(servidor, 100);

//**********Soy una barra separadora :P**************************************//
//**********Acepto las conecciones******************************************//

	struct sockaddr_in direcli;
	unsigned int tamaniodireccion;
	int cliente = accept(servidor, (void*) &direcli, &tamaniodireccion);

		printf("Recibí una conexión en %d!!\n", cliente);
		send(cliente, "Hola NetCat!", 13, 0);
		send(cliente, ":)\n", 4, 0);



	for(;;);

	return 0;
}
