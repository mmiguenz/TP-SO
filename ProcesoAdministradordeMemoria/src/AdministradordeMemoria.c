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
void* deserializarInfoCPU_Inicio_Lectura(int, t_protoc_inicio_lectura_Proceso*,char);
void lecturaMemoria(int,int,MEMORIAPRINCIPAL*,TLB*,t_dictionary*);
void escrituraMemoria(int,int);
void finalizacionProceso(int socketCpu,int socketSwap);
char notificarFinalizarSwap(int socket, t_protoc_Finaliza pedido);
void notificarFinalizarCpu( int socket);
void elminarTablaDePaginasDelProceso(t_dictionary*  tablasPags, int pid);
char escribirContenido(t_protoc_escrituraProceso* pedido, int socketSwap);
void solicitarPagina(t_protoc_escrituraProceso* pedido, int socketSwap);

	 t_config* config;
	 t_log* logAdmMem;
	 t_paramConfigAdmMem* configAdmMem;
	 MEMORIAPRINCIPAL memoriaPrincipal;
	 t_dictionary* tablasPags;
	 TLB* tlb;
	 t_queue* tlb2;
	 int socketSwap;

 int main(void){

	 	 	 	 /* Se levanta el archivo de configuración y se crea log del Administrador de Memoria*/
	 	 	 	 config = config_create("configAdmMem.cfg");
	 	 	 	 configAdmMem = establecerConfigMemoria(config);
	 	 	 	 logAdmMem = log_create("log.txt", "Administrador de memoria",false, LOG_LEVEL_INFO);

	 	 	 	 /* Inicialización de espacio de memoria, array indicador de memoria libre y TLB */

	 	 	 	 t_memoria_crear(&memoriaPrincipal,configAdmMem);


	 	 	 	 char* habilitacionTLB = malloc(sizeof(char)*2);
	 	 	 	 habilitacionTLB = configAdmMem->tlb_habilitada;

	 	 	 	 if (!strcmp(habilitacionTLB,SI)) {
	 	 	 	 tlb = malloc(sizeof(TLB));
	 	 	 	 tlb = t_tlb_crear(configAdmMem);
	 	 	 	 //tlb.CacheTLB=inicializarTLB(configAdmMem->entradas_TLB);
	 	 	 	 }

	 	 	 	 /* Se crea una estructura dinámica que contendrá las tablas de páginas de los procesos en ejecución*/

	 	 	 	 tablasPags = malloc(sizeof(t_dictionary));
	 	 	 	 tablasPags = dictionary_create();

	 	 	 	 /*Conexión del administrador de memoria como cliente al Swap y como Servidor con CPU*/


	 	 	 	 socketSwap = conectar_cliente(configAdmMem->puerto_swap,configAdmMem->ip_swap);
	 	 	 	if(socketSwap>=0)
	 	 	 		 	 	 	{

	 	 	 		 	 	 		printf("Se conecto Al SWAP\n");

	 	 	 		 	 	 	}

	 	 		conectar_servidor(configAdmMem->puerto_escucha, socketSwap);




	 	 	 	 return EXIT_SUCCESS;
 }

 void procesarPedido(int socketCPU, int socketSwap, char tipoInstruccion){


	 switch(tipoInstruccion){

	 	 case INICIAR:{
		 	 inicializacionProceso(socketCPU,socketSwap);
	 	 }
	 	 break;

	 	 case LEER:{
		 	 lecturaMemoria(socketCPU,socketSwap,&memoriaPrincipal,tlb,tablasPags);
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
	 	 t_protoc_inicio_lectura_Proceso* protInic = malloc(sizeof(t_protoc_inicio_lectura_Proceso));
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
	 		 pidConv = string_itoa(protInic->pid);
	 		 crear_e_insertar_TabladePaginas((protInic->paginas), pidConv, tablasPags);
	 		 free(pidConv);
	 	 }

	 	 free(confirmSwap);
	 	 free(confirmCPU);
	 	 free(protInic);
	 	 free(buffer);
 };
 void lecturaMemoria(int socketCPU, int socketSwap, MEMORIAPRINCIPAL* memPrincip, TLB* tlb, t_dictionary* tablasdePagsProcesos){

	 t_protoc_inicio_lectura_Proceso* protInic = malloc(sizeof(t_protoc_inicio_lectura_Proceso));
	 void* buffer = deserializarInfoCPU_Inicio_Lectura(socketCPU,protInic,LEER);

	 // Búsqueda del frame asociado a la página del proceso en TLB
	 int frame;
	 int tamanioContenido;
	 char* contenido = malloc(configAdmMem->tamanio_marco*sizeof(char));
	 frame = buscarPaginaTLB(tlb,protInic->pid,protInic->paginas);
	 if (frame == -1)/*No se encontró página en TLB*/{

		 frame = buscarPaginaenMemoria((protInic->pid),(protInic->paginas),tablasdePagsProcesos);

		 if(frame == -1)/*No se encontró página en MP*/{

			 //enviarSolicitudSwap() y recibirContenido de Swap;
			 send(socketSwap,buffer,sizeof(char)+(sizeof(int)*2),0);
			 recv(socketSwap,&tamanioContenido,sizeof(int),0);
			 recv(socketSwap,contenido,tamanioContenido,0);

			 t_tablaDePaginas* tablaPagsProceso = dictionary_get(tablasdePagsProcesos,string_itoa(protInic->pid));
			 frame = insertarContenidoenMP(socketSwap,contenido,memPrincip,tablaPagsProceso);
			 actualizarTablaPagina_porReemp(frame,protInic->paginas,tablaPagsProceso);

		 }

		 agregar_reemplazarRegistroTLB(tlb,protInic->pid,protInic->paginas,frame);
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

	 free(protInic);
	 free(contenido);
	 free(bufferContAEnviar);

 };


 void escrituraMemoria(int socketCPU, int socketSwap){

	 t_protoc_escrituraProceso* pedido = malloc(sizeof(t_protoc_escrituraProceso));
	 pedido->tipoInstrucc = ESCRIBIR;

	 recv(socketCPU, &(pedido->pid),sizeof(int), 0);
	 recv(socketCPU, &(pedido->pagina),sizeof(int), 0);
	 recv(socketCPU, &(pedido->tamanio),sizeof(int), 0);
	 pedido->contenido = malloc(pedido->tamanio);
	 recv(socketCPU,pedido->contenido, pedido->tamanio,0);


	 char respuesta =  escribirContenido(pedido, socketSwap);

	 send(socketCPU,&respuesta,sizeof(char),0);




 }


 char escribirContenido(t_protoc_escrituraProceso* pedido, int socketSwap)
 {	int frame =-1;

 t_tablaDePaginas* tablaDePaginas =   dictionary_get(tablasPags,string_itoa(pedido->pid));

 	 if(configAdmMem->tlb_habilitada)
 		 frame = buscarPaginaTLB(tlb,pedido->pid,pedido->pagina);



	  frame = (frame <0)?buscarPaginaenMemoria(pedido->pid, pedido->pagina,tablasPags):frame;


	 if(frame>=0)
	 {
		 memoriaPrincipal.Memoria[frame] = pedido->contenido;
		 memoriaPrincipal.MemoriaLibre[frame]= 1;
		 tablaDePaginas->Pagina[pedido->pagina]->bitPresencia=1;
		 tablaDePaginas->Pagina[pedido->pagina]->bitModificado=1;


		 return 1 ;

	 }else
	 {
		 if(t_hayFrameLibre(&memoriaPrincipal, tablaDePaginas,configAdmMem->max_marcos_proceso)<=0)
		 {
			/*
			 * en este punto, quizas tendria sentido que el cpu sepa el motivo de porque se rechazo la lectura.
			 * Sea por la necesidad de reemplazar.
			 * O porque la memoria esta llena.
			 */
			 return 0;

		 }
		 solicitarPagina(pedido,socketSwap);
		 return escribirContenido( pedido, socketSwap);


	 }




 }


 void solicitarPagina(t_protoc_escrituraProceso* pedido, int socketSwap)
 {
	 t_protoc_inicio_lectura_Proceso* pedidoLectura = malloc(sizeof(t_protoc_inicio_lectura_Proceso));
	 pedidoLectura->pid  = pedido->pid;
	 pedidoLectura->paginas = pedido->pagina;
	 pedidoLectura->tipoInstrucc = LEER;

	 void* buffer = malloc(sizeof(t_protoc_inicio_lectura_Proceso));
	 int offset = 0 ;

	 memcpy(buffer,&(pedidoLectura->tipoInstrucc),sizeof(char));
	 offset+=sizeof(int);
	 memcpy(buffer+offset,&(pedidoLectura->paginas),sizeof(int));
	 offset+=sizeof(int);
	 memcpy(buffer+offset,&(pedidoLectura->pid),sizeof(int));


	 send(socketSwap,buffer,sizeof(t_protoc_inicio_lectura_Proceso),0);
	 free(buffer);

	 int tamanioContenido= 0 ;
	 recv(socketSwap,&tamanioContenido,sizeof(int),0);

	 char* contenido = malloc(tamanioContenido);
	 recv(socketSwap,contenido,tamanioContenido,0);



	int frame = t_cargarContenido(&memoriaPrincipal,contenido);

	/*actualizar tabla de paginas paginas */
	t_tablaDePaginas* tabla =  dictionary_get(tablasPags,string_itoa(pedido->pid));

	tabla->Pagina[pedido->pagina]->bitPresencia=1;
	tabla->Pagina[pedido->pagina]->idFrame=frame;
	tabla->Pagina[pedido->pagina]->horaIngreso=90000000;

	agregar_reemplazarRegistroTLB(tlb,pedido->pid,pedido->pagina,frame);


 }



 void finalizacionProceso(int socketCpu,int socketSwap){

	 t_protoc_Finaliza* pedido = malloc(sizeof(t_protoc_Finaliza));


	 pedido->tipoInstrucc = FINALIZAR ;

	 recv(socketCpu,&pedido->pid,sizeof(int),0);


	 if (configAdmMem->tlb_habilitada)
		 t_tlb_limpiar(tlb, pedido->pid);


	 finalizarProceso(&memoriaPrincipal, dictionary_get(tablasPags, string_itoa(pedido->pid)));
	 elminarTablaDePaginasDelProceso(tablasPags,pedido->pid);


	 char respuestaSwap = notificarFinalizarSwap(socketSwap,*pedido);

	 if(respuestaSwap)
	 {

		notificarFinalizarCpu(socketCpu);

	 }


 }


 char notificarFinalizarSwap(int socket, t_protoc_Finaliza pedido)
 {
	 void*buffer = malloc(sizeof(t_protoc_Finaliza));



	 memcpy(buffer,&pedido,sizeof(t_protoc_Finaliza));

	 send(socket,buffer,sizeof(t_protoc_Finaliza),0);

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
	 free(tablaPaginasProceso);
 }

 void* deserializarInfoCPU_Inicio_Lectura(int socketCPU, t_protoc_inicio_lectura_Proceso* protInic_Lect, char InstrSent){

	 void* buffer = malloc(sizeof(char)+sizeof(int)*2);

	 protInic_Lect->tipoInstrucc = InstrSent;

 	 recv(socketCPU,&(protInic_Lect->paginas),sizeof(int),0);
 	 recv(socketCPU,&(protInic_Lect->pid),sizeof(int),0);

 	 memcpy(buffer,&(protInic_Lect->tipoInstrucc),sizeof(char));
 	 int offset = sizeof(char);
 	 memcpy(buffer+offset,&protInic_Lect->pid,sizeof(int));
 	 offset += sizeof(int);
 	 memcpy(buffer+offset,&protInic_Lect->paginas,sizeof(int));

 	 return buffer;

 }


 void elminarTablaDePaginasDelProceso(t_dictionary* tablasPags,int pid)
 {
	 t_tablaDePaginas* tPaginas =   dictionary_get(tablasPags,string_itoa(pid));

	 int i ;
	 for(i = 0; i< tPaginas->cantTotalPaginas;i++)
	 {
		 free(tPaginas->Pagina[i]);
	 }

	 free(tPaginas->Pagina);
	 free(tPaginas);


	dictionary_remove(tablasPags,string_itoa(pid));

 }
