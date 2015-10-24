/*
 ============================================================================
 Name        : ProcesoAdministradordeMemoria.c
 Author      : Gisell Lara
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "AdministradordeMemoria.h"

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
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include "servidor.h"
#include "Cliente.h"
#include "TADConfig.h"
#include "ProtocsyFuncsRecvMsjs.h"
#include "Memoria.h"
#include "Tlb.h"

#define SI "SI"
/*
typedef struct{
	int idFrame;
	char bitPresencia;
	char bitModificado;
}t_regTablaPaginas; */

void crear_e_insertar_TabladePaginas(int, char*, t_dictionary*);
void* deserializarInfoCPU_Inicio_Lectura(int, t_protoc_inicio_lectura_Finaliza_Proceso*,char);
void lecturaMemoria(int,int,MEMORIAPRINCIPAL,TLB*,t_dictionary*);
void escrituraMemoria(int,int);
void finalizacionProceso(int socketCpu,int socketSwap);
char notificarFinalizarSwap(int socket, t_protoc_inicio_lectura_Finaliza_Proceso pedido);
void notificarFinalizarCpu( int socket);


	 t_config* config;
	 t_log* logAdmMem;
	 t_paramConfigAdmMem* configAdmMem;
	 MEMORIAPRINCIPAL memoriaPrincipal;
	 t_dictionary* tablasPags;
	 TLB* tlb;
	 t_queue* tlb2;

 int main(void){

	 	 	 	 /* Se levanta el archivo de configuración y se crea log del Administrador de Memoria*/

	 	 	 	 configAdmMem = establecerConfigMemoria();
	 	 	 	 logAdmMem = log_create("log.txt", "Administrador de memoria",false, LOG_LEVEL_INFO);

	 	 	 	 /* Inicialización de espacio de memoria, array indicador de memoria libre y TLB */

	 	 	 	 memoriaPrincipal.Memoria=inicializarMemoriaPrincipal(configAdmMem->cantidad_marcos,configAdmMem->tamanio_marco);
	 	 	 	 memoriaPrincipal.MemoriaLibre=inicializarMemoriaLibre(configAdmMem->cantidad_marcos);
	 	 	 	 memoriaPrincipal.cantMarcos = configAdmMem->cantidad_marcos;

	 	 	 	 char* habilitacionTLB = malloc(sizeof(char)*2);
	 	 	 	 habilitacionTLB = configAdmMem->tlb_habilitada;

	 	 	 	 if (strcmp(habilitacionTLB,SI)) {
	 	 	 	 tlb = t_tlb_crear(configAdmMem);
	 	 	 	 //tlb.CacheTLB=inicializarTLB(configAdmMem->entradas_TLB);
	 	 	 	 }

	 	 	 	 /* Se crea una estructura dinámica que contendrá las tablas de páginas de los procesos en ejecución*/

	 	 	 	 tablasPags = malloc(sizeof(t_dictionary));
	 	 	 	 tablasPags = dictionary_create();

	 	 	 	 /*Conexión del administrador de memoria como cliente al Swap y como Servidor con CPU*/

	 	 	 	 int swap = conectar_cliente(configAdmMem->puerto_swap,configAdmMem->ip_swap);
	 	 	 	 //conectar_servidor(configAdmMem->puerto_escucha, swap, memoriaPrincipal.MemoriaLibre,Max_Marcos_Por_Proceso,Cant_Marcos,memoriaPrincipal.Memoria);
	 	 	 	 return EXIT_SUCCESS;
 }

 void procesarPedido(int socketCPU, int socketSwap, char tipoInstruccion){
	 enum {INICIAR = 1,LEER,ESCRIBIR,FINALIZAR};

	 switch(tipoInstruccion){

	 	 case INICIAR:{
		 	 inicializacionProceso(socketCPU,socketSwap);
	 	 }
	 	 break;

	 	 case LEER:{
		 	 lecturaMemoria(socketCPU,socketSwap,memoriaPrincipal,tlb,tablasPags);
	 	 }
	 	 break;

	 	 case ESCRIBIR:{
		 	 escrituraMemoria(socketCPU,socketSwap);
	 	 }
	 	 break;

	 	 case FINALIZAR:{
		 	 finalizacionProceso(socketCPU,socketSwap);
	 	 }
	 	 break;
	 }
}

 void inicializacionProceso(int socketCPU, int socketSwap){

	 	 char* confirmSwap = malloc(sizeof(char));
	 	 char* confirmCPU = malloc(sizeof(char));
	 	 char instruccSentencia = 1;

	 	 //Recibo la estructura serializada del CPU y la cargo en estructura correspondiente
	 	 t_protoc_inicio_lectura_Finaliza_Proceso* protInic = malloc(sizeof(t_protoc_inicio_lectura_Finaliza_Proceso));
	 	 void* buffer = deserializarInfoCPU_Inicio_Lectura(socketCPU,protInic,instruccSentencia);

	 	 /* Envío al Swap la info necesaria para que asigne las paginas correspondientes al proceso iniciado */
	 	 send(socketSwap,buffer,sizeof(int)*2,0);
	 	 /* Recibo del Swap la confirmación de asignación de memoria al proceso */
	 	 recv(socketSwap,confirmSwap,sizeof(char),0);
	 	 confirmCPU = confirmSwap;
	 	/* Envío al CPU la señal de asignación o no de memoria al proceso que se inicia */
	 	 send(socketCPU,confirmCPU,sizeof(char),0);

	 	 if (*confirmSwap == 1) {
	 	 //Creación de la tabla de páginas del proceso y agregado de la misma a la Lista de Tablas de Páginas
	 		 char* pidConv = malloc(sizeof(char*));
	 		 pidConv = string_itoa(*(protInic->pid));
	 		 crear_e_insertar_TabladePaginas(*(protInic->paginas), pidConv, tablasPags);
	 	 }

	 	 free(protInic);
 };
 void lecturaMemoria(int socketCPU, int socketSwap, MEMORIAPRINCIPAL memPrincip, TLB* tlb, t_dictionary* tablasdePagsProcesos){

	 t_protoc_inicio_lectura_Finaliza_Proceso* protInic = malloc(sizeof(t_protoc_inicio_lectura_Finaliza_Proceso));
	 char instruccSentencia = 2;
	 void* buffer = deserializarInfoCPU_Inicio_Lectura(socketCPU,protInic,instruccSentencia);

	 // Búsqueda del frame asociado a la página del proceso en TLB
	 int frame;
	 int tamanioContenido;
	 char* contenido = malloc(configAdmMem->tamanio_marco*sizeof(char));
	 frame = buscarPaginaTLB(tlb,*(protInic->pid),*(protInic->paginas));
	 if (frame == -1)/*No se encontró página en TLB*/{

		 frame = buscarPaginaenMemoria(*(protInic->pid),*(protInic->paginas),tablasdePagsProcesos);

		 if(frame == -1)/*No se encontró página en MP*/{

			 //enviarSolicitudSwap() y recibirContenido de Swap;
			 send(socketSwap,buffer,sizeof(char)+(sizeof(int)*2),0);
			 recv(socketSwap,&tamanioContenido,sizeof(int),0);
			 recv(socketSwap,contenido,tamanioContenido,0);

			 t_tablaDePaginas* tablaPagsProceso = dictionary_get(tablasdePagsProcesos,string_itoa(*(protInic->pid)));
			 frame = insertarContenidoenMP(socketSwap,contenido,memPrincip,tablaPagsProceso);
			 //guardarEnMemoria();//Agregar o reemplazar página.
			 //guardarRegistroenTLB();//Reemplazar o agregar registro correspondiente en TLB.

		 }
	 }
	 contenido = memoriaPrincipal.Memoria[frame];

	 // Serializo el contenido del frame leido para su envío al CPU
	 tamanioContenido = strlen(contenido);
	 void* bufferContAEnviar = malloc(sizeof(int)+(sizeof(char)*tamanioContenido)+1);
	 memcpy(bufferContAEnviar,&tamanioContenido,sizeof(int));
	 int offset = sizeof(int);
	 memcpy(bufferContAEnviar+offset,contenido,tamanioContenido+1);
	 offset += tamanioContenido+1;
	 send(socketCPU,bufferContAEnviar,offset,0);







 };
 void escrituraMemoria(int socketCPU, int socketSwap){
 };


 void finalizacionProceso(int socketCpu,int socketSwap){

	 t_protoc_inicio_lectura_Finaliza_Proceso* pedido = malloc(sizeof(t_protoc_inicio_lectura_Finaliza_Proceso));


	 pedido->tipoInstrucc = 4 ;

	 recv(socketCpu,pedido->paginas,sizeof(int),0);
	 recv(socketCpu,pedido->pid,sizeof(int),0);


	 if (configAdmMem->tlb_habilitada)
		 t_tlb_limpiar(&tlb, pedido->pid);


	 finalizarProceso(pedido->pid,&memoriaPrincipal);




	 char respuestaSwap = notificarFinalizarSwap(socketSwap,*pedido);

	 if(respuestaSwap)
	 {

		notificarFinalizarCpu(socketCpu);

	 }


 }


 char notificarFinalizarSwap(int socket, t_protoc_inicio_lectura_Finaliza_Proceso pedido)
 {
	 void*buffer = malloc(sizeof(t_protoc_inicio_lectura_Finaliza_Proceso));



	 memcpy(buffer,&pedido,sizeof(t_protoc_inicio_lectura_Finaliza_Proceso));

	 send(socket,buffer,sizeof(t_protoc_inicio_lectura_Finaliza_Proceso),0);

	 void* bufferResp = malloc(sizeof(char));
	 recv(socket,bufferResp,sizeof(char),0);

	 char resp;

	 memcpy(&resp,bufferResp,sizeof(char));
	 return resp;
 }

 void notificarFinalizarCpu( int socket)
 {

	 void* buffer = malloc(sizeof(char));
	 char respuesta = 1;
	 memcpy(buffer,&respuesta,sizeof(char));
	 send(socket,buffer,sizeof(char),0);



 }



 void crear_e_insertar_TabladePaginas(int paginas, char* pid, t_dictionary* tablasPagsProcesos) {
	 t_tablaDePaginas* tablaPaginasProceso = malloc(sizeof(t_regPagina**)+sizeof(int));
	 tablaPaginasProceso->Pagina = calloc(paginas,sizeof(t_regPagina));
	 tablaPaginasProceso->cantTotalPaginas = paginas;
	 tablaPaginasProceso->pid = atoi(pid);
	 int i;
	 for (i=0; i<paginas; ++i){
	 	 	t_regPagina* tempPagina = malloc(sizeof(t_regPagina));
	 	 	tempPagina->idFrame = -1;
	 	 	tempPagina->bitPresencia = 0;
	 	 	tempPagina->bitModificado = 0;
	 	 	tempPagina->horaIngreso = 90000000;//Espacio que ocupa la hora en formato 'hh:mm:ss:mmmm'
	 	 	tablaPaginasProceso->Pagina[i] = tempPagina;
	 	 	free(tempPagina);
	 };

	 dictionary_put(tablasPagsProcesos,pid,tablaPaginasProceso);
 }

 void* deserializarInfoCPU_Inicio_Lectura(int socketCPU, t_protoc_inicio_lectura_Finaliza_Proceso* protInic_Lect, char InstrSent){

	 void* buffer = malloc(sizeof(char)+sizeof(int)*2);

	 protInic_Lect->tipoInstrucc = InstrSent;

 	 recv(socketCPU,protInic_Lect->paginas,sizeof(int),0);
 	 recv(socketCPU,protInic_Lect->pid,sizeof(int),0);

 	 memcpy(buffer,&(protInic_Lect->tipoInstrucc),sizeof(char));
 	 int offset = sizeof(char);
 	 memcpy(buffer+offset,protInic_Lect->pid,sizeof(int));
 	 offset += sizeof(int);
 	 memcpy(buffer+offset,protInic_Lect->paginas,sizeof(int));

 	 return buffer;

 }
