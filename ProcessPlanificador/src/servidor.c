#include "servidor.h"
#include <sys/types.h>
#include <fcntl.h>
#include "semaph.h"

#include <semaphore.h>
#include <time.h>

sem_t sem_mutex1;
sem_t sem_consumidor;
sem_t sem_consumidor_block;


typedef struct {
char* nombreProc;
int estado;
int PID;
int contadorProgram;
char* path;
int cpu_asignada;
int quantum;//Si el quantum es -1 la planificacion es fifo caso contrario round robin
int retardo_io;
time_t t_entrada_cola_ready;
time_t t_entrada_cola_block;
float tiempo_espera;
int cant_ready;
time_t t_entrada_cola_run;
float tiempo_ejecucion;
int cant_run;
float tiempo_respuesta;

} __attribute__((packed)) PCB ;






PCB *search_and_return(int pid,t_queue * running_PCB);


/*
 * Programa principal.
 * Crea un socket servidor y se mete en un select() a la espera de clientes.
 * Cuando un cliente se conecta, le atiende y lo añade al select() y vuelta
 * a empezar.
 */

void conectar_fifo(char* puerto_escucha_planif,t_queue * fifo_PCB, t_log* logger, t_queue * running_PCB, int mutex, t_queue * block_PCB)
{
	sem_init(&sem_consumidor_block,1,0);
	int socketServidor;				/* Descriptor del socket servidor */
	int socketCliente[MAX_CLIENTES];/* Descriptores de sockets con clientes */
	int numeroClientes = 0;			/* Número clientes conectados */
	fd_set descriptoresLectura;	/* Descriptores de interes para select() */
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

				 if ((recv(socketCliente[i],&header,sizeof(header),0)) > 0){


					 	procesar_mensaje(socketCliente[i],header,fifo_PCB, logger,  running_PCB, mutex,  block_PCB);



				}


				else
				{
					/* Se indica que el cliente ha cerrado la conexión y se
					 * marca con -1 el descriptor para que compactaClaves() lo
					 * elimine */
					printf ("Cliente %d ha cerrado la conexión\n", socketCliente[i]);
					log_trace(logger, "Se ha cerrado la conexion con el CPU: %d", socketCliente[i]);
					socketCliente[i] = -1;

				}
			}
		}

		/* Se comprueba si algún cliente nuevo desea conectarse y se le
		 * admite */
		if (FD_ISSET (socketServidor, &descriptoresLectura)){
			nuevoCliente (socketServidor, socketCliente, &numeroClientes);

//(numeroClientes)-1
			log_trace(logger, "Se conecto exitosamente el CPU: %d", socketCliente[i]);

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
	fd_set master;    // lista de descriptores maestra
    fd_set read_fds;  // lista temporal de descriptores de archivos para el select()


    int listener;     // socket de escucha

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    // nos da el socket y hace bind
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

        //  "direccion en uso" mensaje de error
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // si llegamos aca es que no pudimos hacer el bind
    if (p == NULL) {
        fprintf(stderr, "selectserver: faillo en el  bind\n");

    }

    freeaddrinfo(ai); // listo

    // escucha
    if (listen(listener, 10) == -1) {
        perror("escucha");

    }

	return listener;
}


/****Funcion que recibe un pid de una cola de PCb lo busca y lo destruye****/

 void *search_and_destroy(int pid,t_queue * running_PCB){
	PCB* pcbAux ;

	int tamanio=queue_size(running_PCB);
			while(tamanio>0){
			pcbAux=queue_pop(running_PCB);
			tamanio--;
			if(pcbAux->PID==pid)

			{
			tamanio=0;
			}
			else{
			queue_push(running_PCB, pcbAux);
			}
			}
return 0;

}


procesar_mensaje(int socketCliente,t_msgHeader header,t_queue * fifo_PCB, t_log* logger, t_queue * running_PCB, int mutex, t_queue * block_PCB){

	 PCB_PARCIAL pcb_parc;
					 pcb_parc.contadorDePrograma=0;
					 pcb_parc.pid=0;
					 pcb_parc.tiempo=0;


	printf("-------------------EL MSJ type es %d \n",header.msgtype);

	switch(header.msgtype){

	case 0 : {

	log_trace(logger,"CPU %d esta libre\n", header.payload_size);

	printf ("CPU %d esta libre\n", header.payload_size);


	PCB* PcbAux;
	PcbAux=malloc(sizeof(PCB*));

	sem_wait(&sem_mutex1);
	sem_wait(&sem_consumidor);
	PcbAux=queue_peek(fifo_PCB);
	PcbAux->cpu_asignada=socketCliente;
	sem_post(&sem_consumidor);
	sem_post(&sem_mutex1);
	char* mensaje;
	mensaje= malloc(sizeof(int)+sizeof(int)+sizeof(int)+sizeof(int)+strlen(PcbAux->path)+strlen(PcbAux->nombreProc)+2+sizeof(t_msgHeader));

	log_trace(logger,"\n PCB a mandar \n\n");
	printf("\n PCB a mandar \n\n");
	log_trace(logger,"EL nombredelproceso es........: %s \n",PcbAux->nombreProc);
	printf("EL nombredelproceso es........: %s \n",PcbAux->nombreProc);
	log_trace(logger,"EL Path es........: %s \n",PcbAux->path);
	printf("EL Path es........: %s \n",PcbAux->path);
	log_trace(logger,"EL PID es........: %d \n ---------------------------\n",PcbAux->PID);
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


	memcpy(mensaje +offset  , &(PcbAux->quantum), sizeof(int));
	offset+=sizeof(int);

	memset(&header, 0, sizeof(t_msgHeader)); // Ahora el struct tiene cero en todos sus miembros
	header.msgtype = 1;//MSG_PCB;
	header.payload_size = offset;
	send(socketCliente,&header,sizeof(header),0);
	send(socketCliente,mensaje,header.payload_size,0);

	free(mensaje);

	recv(socketCliente,&header,sizeof(header),0);

	if(header.msgtype == 2)
	{
	printf("El proceso inicio correctamente \n");
	log_trace(logger, "Se ha iniciado el proceso con el CPU: %d", socketCliente);

	PCB* PcbRun=malloc(sizeof(PCB));
	PcbRun->path=malloc(strlen(PcbAux->path)+1);
	PcbRun->nombreProc=malloc(strlen(PcbAux->nombreProc)+1);

	//free(PcbAux);

	sem_wait(&sem_mutex1);
	sem_wait(&sem_consumidor);
	PcbRun=queue_pop(fifo_PCB);
	PcbRun->cpu_asignada=socketCliente;
	PcbRun->cant_ready++;
	PcbRun->tiempo_espera=((PcbRun->tiempo_espera)+(difftime(time(NULL),PcbRun->t_entrada_cola_ready)));
	PcbRun->t_entrada_cola_run=time(NULL);
	queue_push(running_PCB,PcbRun);
	sem_post(&sem_mutex1);
	}
	else{

		printf("El proceso %d fallo \n",header.payload_size);
		log_warning(logger, "Se ha finalizado el proceso con el CPU: %d debido a un fallo", socketCliente);
		recv(socketCliente, &pcb_parc,sizeof (PCB_PARCIAL),0);

			sem_wait(&sem_consumidor);
			queue_pop(fifo_PCB);



								break;


	}
	//free(PcbRun);




	break;
	}

	case 3 : {
		PCB* PcbFin;
		PcbFin=malloc(sizeof(PCB*));
		PcbFin->nombreProc=malloc(50);
		PcbFin->path=malloc(200);

	printf("El proceso %d finalizo correctamente \n",header.payload_size);
	log_trace(logger, "Se ha finalizado el proceso con el CPU: %d", socketCliente);
	recv(socketCliente, &pcb_parc,sizeof (PCB_PARCIAL),0);

	PcbFin=search_and_return(pcb_parc.pid,running_PCB);
	PcbFin->cant_run++;
	PcbFin->tiempo_ejecucion=((PcbFin->tiempo_ejecucion)+(difftime(time(NULL),PcbFin->t_entrada_cola_run)));
	log_info(logger,"-----------El Tiempo de espera fue de %.2f segundos aproximadamente--------\n",PcbFin->tiempo_espera);
	printf("-----------El Tiempo de espera fue de %.2f segundos aproximadamente--------\n",PcbFin->tiempo_espera);
	log_info(logger,"-----------El Tiempo de ejecucion fue de %.2f segundos aproximadamente......\n\n",PcbFin->tiempo_ejecucion);
	printf("-----------El Tiempo de ejecucion fue de %.2f segundos aproximadamente......\n\n",PcbFin->tiempo_ejecucion);
	log_info(logger,"-----------El Tiempo de respuesta fue de %.2f segundos aproximadamente......\n\n",PcbFin->tiempo_respuesta);
	printf("-----------El Tiempo de respuesta fue de %.2f segundos aproximadamente......\n\n",PcbFin->tiempo_respuesta);

	break;
						}

	case 4://Entrada salida
	{
		recv(socketCliente, &pcb_parc,sizeof (PCB_PARCIAL),0);
		PCB* Pcb_IO=malloc(sizeof(PCB*));
		 Pcb_IO->nombreProc=malloc(50);
		 Pcb_IO->path=malloc(200);
		Pcb_IO=search_and_return(pcb_parc.pid,running_PCB);
		printf("El proceso a blokear es........ %s",Pcb_IO->nombreProc );
		log_trace(logger,"El proceso a blokear es........ %s\n",Pcb_IO->nombreProc );
		Pcb_IO->retardo_io=pcb_parc.tiempo;
		Pcb_IO->contadorProgram= pcb_parc.contadorDePrograma;
		Pcb_IO->cant_run++;
		Pcb_IO->tiempo_ejecucion=((Pcb_IO->tiempo_ejecucion)+(difftime(time(NULL),Pcb_IO->t_entrada_cola_run)));
		Pcb_IO->t_entrada_cola_block=time(NULL);
		queue_push(block_PCB,Pcb_IO);
		sem_post(&sem_consumidor_block);




		break;
	}
	case 5://Termina por quantum
	{
		PCB* pcbAux= malloc(sizeof (PCB));
		pcbAux->path=malloc(50);
		pcbAux->nombreProc=malloc(300);
		recv(socketCliente, &pcb_parc,sizeof (PCB_PARCIAL),0);
		pcbAux->path=malloc(50);
		pcbAux->nombreProc=malloc(300);
		pcbAux=search_and_return(pcb_parc.pid,running_PCB);
		pcbAux->contadorProgram=pcb_parc.contadorDePrograma;
		pcbAux->t_entrada_cola_ready=time(NULL);
		pcbAux->cant_run++;
		pcbAux->tiempo_ejecucion=((pcbAux->tiempo_ejecucion)+(difftime(time(NULL),pcbAux->t_entrada_cola_run)));

		queue_push(fifo_PCB,pcbAux);
		sem_post(&sem_consumidor);

		break;

	}
	}

return 0;
}


PCB *search_and_return(int pid,t_queue * running_PCB){
	PCB* pcbAux=malloc(sizeof(PCB));
	pcbAux->nombreProc=malloc(50);
	pcbAux->path=malloc(200);

	int tamanio=queue_size(running_PCB);
				while(tamanio!=0){
				pcbAux=queue_pop(running_PCB);
				tamanio--;
				if(pcbAux->PID==pid)
				{
				tamanio=0;
				}
				else{
				queue_push(running_PCB, pcbAux);
				}
				}
	return pcbAux;

	}

