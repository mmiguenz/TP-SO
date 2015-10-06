#include "servidor.h"


#include <semaphore.h>
sem_t sem_productor;
sem_t sem_consumidor;

#define LIBRE 0
#define INICIO 2
#define FINALIZO 3
#define MSG_PCB 1


typedef struct {
char* nombreProc;
int estado;
int PID;
int contadorProgram;
char* path;
int cpu_asignada;

}PCB ;

typedef struct  {
	int msgtype;
	int payload_size;
}t_msgHeader;

t_queue * fifo_PCB_running; //Cola de pcb que estan corriendose

/*
 * Programa principal.
 * Crea un socket servidor y se mete en un select() a la espera de clientes.
 * Cuando un cliente se conecta, le atiende y lo añade al select() y vuelta
 * a empezar.
 */
void conectar_fifo(char* puerto_escucha_planif,t_queue * fifo_PCB, t_log* logger)
{

	fifo_PCB_running=queue_create();
	int socketServidor;				/* Descriptor del socket servidor */
	int socketCliente[MAX_CLIENTES];/* Descriptores de sockets con clientes */
	int numeroClientes = 0;			/* Número clientes conectados */
	fd_set descriptoresLectura;	/* Descriptores de interes para select() */
	//char* buffer;							/* Buffer para leer de los socket */
	int maximo;							/* Número de descriptor más grande */
	int i;								/* Para bubles */

	/* Se abre el socket servidor, avisando por pantalla y saliendo si hay
	 * algún problema */

	socketServidor = Abre_Socket_Inet (puerto_escucha_planif);//"cpp_java");
	if (socketServidor == -1)
	{
		perror ("Error al abrir servidor");

	}

	/* Bucle infinito.
	 * Se atiende a si hay más clientes para conectar y a los mensajes enviados
	 * por los clientes ya conectados */

	while (1)
	{
		/* Cuando un cliente cierre la conexión, se pondrá un -1 en su descriptor
		 * de socket dentro del array socketCliente. La función compactaClaves()
		 * eliminará dichos -1 de la tabla, haciéndola más pequeña.
		 *
		 * Se eliminan todos los clientes que hayan cerrado la conexión */

		compactaClaves (socketCliente, &numeroClientes);

		/* Se inicializa descriptoresLectura */

		FD_ZERO (&descriptoresLectura);

		/* Se añade para select() el socket servidor */

		FD_SET (socketServidor, &descriptoresLectura);

		/* Se añaden para select() los sockets con los clientes ya conectados */

		for (i=0; i<numeroClientes; i++)
			FD_SET (socketCliente[i], &descriptoresLectura);

		/* Se el valor del descriptor más grande. Si no hay ningún cliente,
		 * devolverá 0 */
		maximo = dameMaximo (socketCliente, numeroClientes);

		if (maximo < socketServidor)
			maximo = socketServidor;

		/* Espera indefinida hasta que alguno de los descriptores tenga algo
		 * que decir: un nuevo cliente o un cliente ya conectado que envía un
		 * mensaje */

		select (maximo + 1, &descriptoresLectura, NULL, NULL, NULL);

		/* Se comprueba si algún cliente ya conectado ha enviado algo */

		for (i=0; i<numeroClientes; i++)
		{
			if (FD_ISSET (socketCliente[i], &descriptoresLectura))
			{
				/* Se lee lo enviado por el cliente y se escribe en pantalla */
				 t_msgHeader header;
				 header.msgtype=0;
				 header.payload_size=0;
				 //memset(&header, 0, sizeof(t_msgHeader));



				if ((recv(socketCliente[i],&header,sizeof(header),0)) > 0){
					printf("-------------------EL MSJ type es %d \n",header.msgtype);
					switch(header.msgtype){
					case LIBRE : {
					printf ("CPU %d esta libre\n", header.payload_size);
					PCB* PcbAux;



					sem_wait(&sem_consumidor);
					PcbAux=queue_pop(fifo_PCB);
					char* mensaje;
					mensaje= malloc(sizeof(int)+sizeof(int)+sizeof(int)+strlen(PcbAux->path)+strlen(PcbAux->nombreProc)+2+sizeof(t_msgHeader));

					printf("\n PCB a mandar \n\n");
					printf("EL nombredelproceso es........: %s \n",PcbAux->nombreProc);
					printf("EL Path es........: %s \n",PcbAux->path);
					printf("EL PID es........: %d \n",PcbAux->PID);
					printf("----------------------------------- \n");
					int offset=0;
					memcpy(mensaje +offset  , &(PcbAux->PID), sizeof(int));
					offset+=sizeof(int);
					memcpy(mensaje +offset  , &(PcbAux->contadorProgram), sizeof(int));
					offset+=sizeof(int);
					memcpy(mensaje +offset  , &(PcbAux->cpu_asignada), sizeof(int));
					offset+=sizeof(int);
					memcpy(mensaje +offset  , PcbAux->path, strlen(PcbAux->path)+1);
					offset+=strlen(PcbAux->path)+1;
					memcpy(mensaje +offset  , PcbAux->nombreProc, strlen(PcbAux->nombreProc)+1);
					offset+=strlen(PcbAux->nombreProc)+1;
					memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
					header.msgtype = MSG_PCB;
					header.payload_size = offset;
					send(socketCliente[i],&header,sizeof(header),0);
					send(socketCliente[i],mensaje,header.payload_size,0);
					PcbAux->cpu_asignada=socketCliente[i];
					queue_push(fifo_PCB_running,PcbAux);
					free(mensaje);
					free(PcbAux);

					break;
					}
					case INICIO : {
						printf("El proceso inicio correctamente \n");
						log_info(logger, "Se ha iniciado el proceso con el CPU: %d", socketCliente[i]);

						//queue_push(fifo_PCB_running,PcbAux);
							//				free(PcbAux);

						break;
					}
					case FINALIZO : {
											printf("El proceso %d finalizo correctamente \n",header.payload_size);
											log_info(logger, "Se ha iniciado el proceso con el CPU: %d", socketCliente[i]);

											//queue_push(fifo_PCB_running,PcbAux);
												//				free(PcbAux);

											break;
										}
					}



				}


				else
				{
					/* Se indica que el cliente ha cerrado la conexión y se
					 * marca con -1 el descriptor para que compactaClaves() lo
					 * elimine */
					printf ("Cliente %d ha cerrado la conexión\n", socketCliente[i]);
					log_info(logger, "Se ha cerrado la conexion con el CPU: %d", socketCliente[i]);
					socketCliente[i] = -1;

				}
			}
		}

		/* Se comprueba si algún cliente nuevo desea conectarse y se le
		 * admite */
		if (FD_ISSET (socketServidor, &descriptoresLectura)){
			nuevoCliente (socketServidor, socketCliente, &numeroClientes);

//(numeroClientes)-1
					 log_info(logger, "Se conecto exitosamente el CPU: %d", socketCliente[i]);
				//mensaje=malloc(sizeof(char*));

	}}


}

/*
 * Crea un nuevo socket cliente.
 * Se le pasa el socket servidor y el array de clientes, con el número de
 * clientes ya conectados.
 */
void nuevoCliente (int servidor, int *clientes, int *nClientes)
{
	/* Acepta la conexión con el cliente, guardándola en el array */
	clientes[*nClientes] = Acepta_Conexion_Cliente (servidor);
	(*nClientes)++;


	if ((*nClientes) >= MAX_CLIENTES)
	{
		close (clientes[(*nClientes) -1]);
		(*nClientes)--;
		return;
	}

	/* Envía su número de cliente al cliente */
	Escribe_Socket (clientes[(*nClientes)-1], "Te conectaste al planificador", strlen("Te conectaste al planificador"));

	/* Escribe en pantalla que ha aceptado al cliente y vuelve */
	printf ("Aceptado cpu %d\n", *nClientes);
	return;
}

/*
 * Función que devuelve el valor máximo en la tabla.
 * Supone que los valores válidos de la tabla son positivos y mayores que 0.
 * Devuelve 0 si n es 0 o la tabla es NULL */

int dameMaximo (int *tabla, int n)
{
	int i;
	int max;

	if ((tabla == NULL) || (n<1))
		return 0;

	max = tabla[0];
	for (i=0; i<n; i++)
		if (tabla[i] > max)
			max = tabla[i];

	return max;
}

/*
 * Busca en array todas las posiciones con -1 y las elimina, copiando encima
 * las posiciones siguientes.
 * Ejemplo, si la entrada es (3, -1, 2, -1, 4) con *n=5
 * a la salida tendremos (3, 2, 4) con *n=3
 */
void compactaClaves (int *tabla, int *n)
{
	int i,j;

	if ((tabla == NULL) || ((*n) == 0))
		return;

	j=0;
	for (i=0; i<(*n); i++)
	{
		if (tabla[i] != -1)
		{
			tabla[j] = tabla[i];
			j++;
		}
	}

	*n = j;
}

/*
* Escribe dato en el socket cliente. Devuelve numero de bytes escritos,
* o -1 si hay error.
*/
int Escribe_Socket (int fd, char *Datos, int Longitud)
{
	int Escrito = 0;
	int Aux = 0;

	/*
	* Comprobacion de los parametros de entrada
	*/
	if ((fd == -1) || (Datos == NULL) || (Longitud < 1))
		return -1;

	/*
	* Bucle hasta que hayamos escrito todos los caracteres que nos han
	* indicado.
	*/
	while (Escrito < Longitud)
	{
		Aux = write (fd, Datos + Escrito, Longitud - Escrito);
		if (Aux > 0)
		{
			/*
			* Si hemos conseguido escribir caracteres, se actualiza la
			* variable Escrito
			*/
			Escrito = Escrito + Aux;
		}
		else
		{
			/*
			* Si se ha cerrado el socket, devolvemos el numero de caracteres
			* leidos.
			* Si ha habido error, devolvemos -1
			*/
			if (Aux == 0)
				return Escrito;
			else
				return -1;
		}
	}

	/*
	* Devolvemos el total de caracteres leidos
	*/
	return Escrito;
}

/*
* Se le pasa un socket de servidor y acepta en el una conexion de cliente.
* devuelve el descriptor del socket del cliente o -1 si hay problemas.
* Esta funcion vale para socket AF_INET o AF_UNIX.
*/
int Acepta_Conexion_Cliente (int Descriptor)
{
	socklen_t Longitud_Cliente;
	struct sockaddr Cliente;
	int Hijo;

	/*
	* La llamada a la funcion accept requiere que el parametro
	* Longitud_Cliente contenga inicialmente el tamano de la
	* estructura Cliente que se le pase. A la vuelta de la
	* funcion, esta variable contiene la longitud de la informacion
	* util devuelta en Cliente
	*/
	Longitud_Cliente = sizeof (Cliente);
	Hijo = accept (Descriptor, &Cliente, &Longitud_Cliente);
	if (Hijo == -1)
		return -1;

	/*
	* Se devuelve el descriptor en el que esta "enchufado" el cliente.
	*/
	return Hijo;
}

/*
* Abre un socket servidor de tipo AF_INET. Devuelve el descriptor
*	del socket o -1 si hay probleamas
* Se pasa como parametro el nombre del servicio. Debe estar dado
* de alta en el fichero /etc/services
*/
int Abre_Socket_Inet (char* puerto_escucha_planif)
{
	fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()


    int listener;     // listening socket descriptor

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, puerto_escucha_planif, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));

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

    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");

    }

	return listener;
}



