
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

void error(int code, char *err);
int AtiendeCliente(int socket, struct sockaddr_in addr);
void reloj(int loop);


/** Número máximo de hijos */
#define MAX_CHILDS 3

int main(void) {

//**********Soy una barra inicial********************************************//
//**********Estructuras para el socket**************************************//


    struct sockaddr_in servidor_addr;
    struct sockaddr_in client_addr;
    struct sockaddr_in my_addr;


    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_addr.s_addr = INADDR_ANY;
    servidor_addr.sin_port = htons(8080);

    int activated=1;
    int loop=0;
    struct timeval tv;      /* Para el timeout del accept */
    socklen_t size_addr = 0;
    int socket_client;
    fd_set rfds;        /* Conjunto de descriptores a vigilar */
    int childcount=0;
    int exitcode;

    int childpid;
    int pidstatus;

 //**********Soy una barra separadora :P**************************************//
//**********Creo la coneccion del soket**************************************//

	int servidor = socket(AF_INET, SOCK_STREAM, 0);

	if(servidor == -1)
      error(1, "No puedo inicializar el socket");

	int activado = 1;

	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if (bind(servidor, (void*) &servidor_addr, sizeof(servidor_addr)) == -1) {
		perror("Falló el bind");
		return 1;
	}

	if(listen( servidor, 10) == -1 )
	      error(3, "No puedo escuchar en el puerto especificado");

	printf("Estoy escuchando\n");

	size_addr = sizeof(struct sockaddr_in);


//**********Soy una barra separadora :P**************************************//
//**********Acepto las conecciones******************************************//

	while(activated)
	      {
	    reloj(loop);
	    /* select() se carga el valor de rfds */
	    FD_ZERO(&rfds);
	    FD_SET(servidor, &rfds);

	    /* select() se carga el valor de tv */
	    tv.tv_sec = 0;
	    tv.tv_usec = 500000;    /* Tiempo de espera */

	    if (select(servidor+1, &rfds, NULL, NULL, &tv))
	      {
	        if((socket_client = accept( servidor, (struct sockaddr*)&client_addr, &size_addr))!= -1)
	          {
	        loop=-1;        /* Para reiniciar el mensaje de Esperando conexión... */
	        printf("\nSe ha conectado %s por su puerto %d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
	        switch ( childpid=fork() )
	          {
	          case -1:  /* Error */
	            error(4, "No se puede crear el proceso hijo");
	            break;
	          case 0:   /* Somos proceso hijo */
	              exitcode=AtiendeCliente(socket_client, client_addr);

	            exit(exitcode); /* Código de salida */
	          default:  /* Somos proceso padre */
	            childcount++; /* Acabamos de tener un hijo */
	            close(socket_client); /* Nuestro hijo se las apaña con el cliente que
	                         entró, para nosotros ya no existe. */
	            break;
	          }
	          }
	        else
	          fprintf(stderr, "ERROR AL ACEPTAR LA CONEXIÓN\n");
	      }

	    /* Miramos si se ha cerrado algún hijo últimamente */
	       childpid=waitpid(0, &pidstatus, WNOHANG);
	       if (childpid>0)
	         {
	           childcount--;   /* Se acaba de morir un hijo */

	           /* Muchas veces nos dará 0 si no se ha muerto ningún hijo, o -1 si no tenemos hijos
	            con errno=10 (No child process). Así nos quitamos esos mensajes*/

	           if (WIFEXITED(pidstatus))
	             {

	           /* Tal vez querremos mirar algo cuando se ha cerrado un hijo correctamente */
	           if (WEXITSTATUS(pidstatus)==99)
	             {
	               printf("\nSe ha pedido el cierre del programa\n");
	               activated=0;
	             }
	             }
	         }
	       loop++;
	       }

	       close(servidor);

	       return 0;
	   }



//**********Soy una barra separadora :P**************************************//
//**********Funciones********************************************************//

int AtiendeCliente(int socket, struct sockaddr_in addr)
{
	send(socket, "Hola", 5, 0);
	return 1;
}


void error(int code, char *err)
{
  char *msg=(char*)malloc(strlen(err)+14);
  sprintf(msg, "Error %d: %s\n", code, err);
  fprintf(stderr, msg);
  exit(1);
}

void reloj(int loop)
{
  if (loop==0)
    printf("[SERVIDOR] Esperando conexión  ");

  printf("\033[1D");        /* Introducimos código ANSI para retroceder 2 caracteres */
  switch (loop%4)
    {
    case 0: printf("|"); break;
    case 1: printf("/"); break;
    case 2: printf("-"); break;
    case 3: printf("\\"); break;
    default:            /* No debemos estar aquí */
      break;
    }

  fflush(stdout);       /* Actualizamos la pantalla */
}
