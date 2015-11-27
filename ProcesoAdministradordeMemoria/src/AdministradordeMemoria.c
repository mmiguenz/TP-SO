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
#include <signal.h>
#include <semaphore.h>
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
#include "LoggingAdmMem.h"

#define MAXTIME 90000000;


void crear_e_insertar_TabladePaginas(int, int, t_dictionary*);
void* deserializarInfoCPU_Inicio_Lectura(int, t_protoc_inicio_lectura_Proceso*,char);
void lecturaMemoria(int,int);
void escrituraMemoria(int,int);
void finalizacionProceso(int socketCpu,int socketSwap);
char notificarFinalizarSwap(int socket, t_protoc_Finaliza* pedido);
void notificarFinalizarCpu( int socket);
void eliminarTablaDePaginasDelProceso(t_dictionary*  tablasPags, int pid);
void tlbFlush();
void memFlush();
void tlbFlushHandler(int signum);
void memoriaFlushHandler(int signum);
void dumpMemoriaHandler(int signum);
void memoryDump();
void publicarTasaAciertos();
void* prepararSolicitudASwap(t_protoc_escrituraProceso* pedido);
char* solicitarContenidoASwap(int socketSwap,void* buffer);

t_config* config;
t_log* logAdmMem;
t_paramConfigAdmMem* configAdmMem;
MEMORIAPRINCIPAL* memoriaPrincipal;
t_dictionary* tablasPags;
TLB* tlb;
pthread_t hilo_tlbFlush;
pthread_t hilo_memFlush;
pthread_t hilo_tasaHitsTLB;
pthread_attr_t attr;
pthread_mutex_t mutex;
pthread_mutex_t mutexTLB;
int socketSwap;
pid_t pid;
float referMem_TLB;
float aciertos_TLB;

 int main(void){

	 	 	 	 /* Se levanta el archivo de configuración y se crea log del Administrador de Memoria*/
	 	 	 	 config = config_create("configAdmMem.cfg");
	 	 	 	 configAdmMem = establecerConfigMemoria(config);
	 	 	 	 logAdmMem = log_create("AdmMemoria.log", "Administrador de memoria",false, LOG_LEVEL_INFO);

	 	 	 	 /* Inicialización de espacio de memoria, array indicador de memoria libre y TLB */

	 	 	 	memoriaPrincipal =  t_memoria_crear(configAdmMem);

	 	 	 	 if (configAdmMem->tlb_habilitada) {
	 	 	        tlb = t_tlb_crear(configAdmMem);
	 	 	        referMem_TLB = 0;
	 	 	        aciertos_TLB = 0;

	 	 	        pthread_create(&hilo_tasaHitsTLB, NULL,(void*)publicarTasaAciertos,NULL);
	 	 	 	 }


	 	 	 	 /* Se crea una estructura dinámica que contendrá las tablas de páginas de los procesos en ejecución*/

	 	 	 	 tablasPags = malloc(sizeof(t_dictionary*));
	 	 	 	 tablasPags = dictionary_create();

	 	 	 	 /* Creación de los hilos para los flush de TLB y memoria */

	 	 	 	 signal(SIGUSR2,memoriaFlushHandler);
	 	 	 	 signal(SIGUSR1,tlbFlushHandler);
	 	 	 	 signal(SIGPOLL,dumpMemoriaHandler);


	 	 	 	 pthread_mutexattr_t Attr;
	 	 	 	 pthread_mutexattr_init(&Attr);
	 	 	 	 pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
	 	 	 	 pthread_mutex_init(&mutex, &Attr);
	 	 	 	 pthread_mutex_init(&mutex,&Attr);



	 	 	 	 /*Conexión del administrador de memoria como cliente al Swap y como Servidor con CPU*/

	 	 	 	 socketSwap = conectar_cliente(configAdmMem->puerto_swap,configAdmMem->ip_swap);

	 	 	 	 conectar_servidor(configAdmMem->puerto_escucha, socketSwap);


	 	 	 	 return EXIT_SUCCESS;
 }


 void procesarPedido(int socketCPU, int socketSwap, char tipoInstruccion){

	 pthread_mutex_lock(&mutex);

	 switch(tipoInstruccion){
	 	 case INICIAR:{
		 	 inicializacionProceso(socketCPU,socketSwap);
	 	 }
	 	 break;

	 	 case LEER:{
		 	 lecturaMemoria(socketCPU,socketSwap);
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
	 pthread_mutex_unlock(&mutex);


}

 void inicializacionProceso(int socketCPU, int socketSwap){

	 	 char* confirmSwap = malloc(sizeof(char));
	 	 char* confirmCPU;
	 	 char instruccSentencia = 1;

	 	 //Recibo la estructura serializada del CPU y la cargo en estructura correspondiente
	 	 t_protoc_inicio_lectura_Proceso* protInic = malloc(sizeof(t_protoc_inicio_lectura_Proceso));
	 	 void* buffer = deserializarInfoCPU_Inicio_Lectura(socketCPU,protInic,instruccSentencia);

	 	 /* Envío al Swap la info necesaria para que asigne las paginas correspondientes al proceso iniciado */
	 	 send(socketSwap,buffer,sizeof(t_protoc_inicio_lectura_Proceso),0);
	 	 /* Recibo del Swap la confirmación de asignación de memoria al proceso */
	 	 recv(socketSwap,confirmSwap,sizeof(char),0);
	 	 confirmCPU = confirmSwap;
	 	/* Envío al CPU la señal de asignación o no de memoria al proceso que se inicia */
	 	 send(socketCPU,confirmCPU,sizeof(char),0);

	 	 if (*confirmSwap == 1) {
	 	 //Creación de la tabla de páginas del proceso y agregado de la misma a la Lista de Tablas de Páginas
	 		 crear_e_insertar_TabladePaginas((protInic->paginas), protInic->pid, tablasPags);
	 		 sleep(configAdmMem->retardo_memoria);
	 	 }

	 	 //---------Logging creacion de MProc--------------------//
	 	 t_tempLogueo* tempLog = cargaDatosLogCrearMProc(protInic);
	 	 loguearEvento(logAdmMem,tempLog);
	 	 //------------------------------------------------------//

	 	 free(confirmSwap);
	 	 free(protInic);
	 	 free(buffer);
 };
 void lecturaMemoria(int socketCPU, int socketSwap){

	 t_protoc_inicio_lectura_Proceso* protInic = malloc(sizeof(t_protoc_inicio_lectura_Proceso));
	 void* buffer = deserializarInfoCPU_Inicio_Lectura(socketCPU,protInic,LEER);

	 //---------Logging de solicitud de lectura-------------//
	 t_tempLogueo* tempLog = cargaDatosLogSolicLect(protInic);
	 loguearEvento(logAdmMem,tempLog);
	 //-----------------------------------------------------//

	 // Búsqueda del frame asociado a la página del proceso en TLB
	 int frame=-1;
	 int marcosAsignados;
	 int tamanioContenido;
	 char* contenido;
	 int entradaTLB;
	 int paginaReemp = 0;
	 int tlbHit = false;

	 char* pidBuscado = string_itoa(protInic->pid);
	 t_tablaDePaginas* tablaPagsProceso = malloc(sizeof(t_tablaDePaginas));
	 tablaPagsProceso = dictionary_get(tablasPags,pidBuscado);

	 frame = configAdmMem->tlb_habilitada?buscarPaginaTLB(tlb,protInic->pid,protInic->paginas,&entradaTLB):frame;

	 pthread_mutex_lock(&mutexTLB);
	 referMem_TLB++; // Cantidad de referencias a memoria históricas (para calcular tasa de aciertos TLB)
	 pthread_mutex_unlock(&mutexTLB);

	 if(frame != -1){
		 actualizarUtilizyModifPag(LEER,tablaPagsProceso,protInic->paginas);
	 //----Logging Acceso TLB(hit) -------//
		 t_tempLogueo* datosLogTLB = cargaDatosAccesoTLB(protInic->pid,protInic->paginas,frame,entradaTLB);
		 datosLogTLB->hit = true;
		 tlbHit = datosLogTLB->hit;
		 loguearEvento(logAdmMem,datosLogTLB);
	 //-----------------------------------//
		 pthread_mutex_lock(&mutexTLB);
		 aciertos_TLB++; // Modificación de cantidad histórica de aciertos TLB
		 pthread_mutex_unlock(&mutexTLB);
	 }

	 if (frame == -1 && tlbHit != true )/*No se encontró página en TLB*/{

		 frame = buscarPaginaenMemoria(protInic->pid,protInic->paginas,tablasPags);
		 sleep(configAdmMem->retardo_memoria);

		 if(frame != -1){
			 actualizarUtilizyModifPag(LEER,tablaPagsProceso,protInic->paginas);
		 //-----------Logging Acceso Memoria (hit)--------//
			 t_tempLogueo* datosLogMemoria = cargaDatosAccesoMemoria(protInic->pid,protInic->paginas,frame);
			 datosLogMemoria->hit = true;
			 loguearEvento(logAdmMem,datosLogMemoria);
		 //-----------------------------------------------//
		 }
	 }



		 if(frame == -1 && tlbHit != true)/*No se encontró página en MP*/{

			contenido = solicitarContenidoASwap(socketSwap,buffer);

			 (tablaPagsProceso->contadorPF)++;
			 marcosAsignados = marcosUtilizadosProceso(tablaPagsProceso);
			 frame = buscarFrameLibre(memoriaPrincipal);

			 if (marcosAsignados == configAdmMem->max_marcos_proceso){
				 frame = reemplazarPagina(configAdmMem,socketSwap,memoriaPrincipal,tablaPagsProceso,&paginaReemp,protInic->paginas);
				 borrarRegistroTLBPagReemp(tlb,&paginaReemp,protInic->pid);
				 insertarPaginaenMP(contenido,memoriaPrincipal,&frame);
				 actualizarTablaPaginas(LEER,frame,protInic->paginas,tablaPagsProceso);
				 sleep(configAdmMem->retardo_memoria);
				 loguearActualizacionMemoria(logAdmMem,protInic->paginas,paginaReemp,protInic->pid,frame);
			 }
			 else{
			 if (frame == -1){
				 if(marcosAsignados == 0) {
					finalizarProceso(memoriaPrincipal, dictionary_get(tablasPags, string_itoa(protInic->pid)));
					eliminarTablaDePaginasDelProceso(tablasPags,protInic->pid);
					t_protoc_Finaliza* protFinaliz = malloc(sizeof(t_protoc_Finaliza));
					protFinaliz->tipoInstrucc = FINALIZAR;
					protFinaliz->pid = protInic->pid;
					int rtaSwap = (int)notificarFinalizarSwap(socketSwap,protFinaliz);
					rtaSwap = -1;
					send(socketCPU,&rtaSwap,sizeof(int),0);
			 	 }
			 	 else {
					frame = reemplazarPagina(configAdmMem,socketSwap,memoriaPrincipal,tablaPagsProceso,&paginaReemp,protInic->paginas);
					borrarRegistroTLBPagReemp(tlb,&paginaReemp,protInic->pid);
					insertarPaginaenMP(contenido,memoriaPrincipal,&frame);
					actualizarTablaPaginas(LEER,frame,protInic->paginas,tablaPagsProceso);
					sleep(configAdmMem->retardo_memoria);
					loguearActualizacionMemoria(logAdmMem,protInic->paginas,paginaReemp,protInic->pid,frame);
			 	 }
			 }
			 else {

				 insertarPaginaenMP(contenido,memoriaPrincipal,&frame);
				 actualizarTablaPaginas(LEER,frame,protInic->paginas,tablaPagsProceso);
				 actualizarVectorClockModif(configAdmMem,tablaPagsProceso,protInic->paginas);
				 sleep(configAdmMem->retardo_memoria);
				 paginaReemp = -1;
				 loguearActualizacionMemoria(logAdmMem,protInic->paginas,paginaReemp,protInic->pid,frame);
			 	 }
			 }
		 }

		 (tablaPagsProceso->contadorPaginasAcc)++;

		 if (frame != -1){

			 if (configAdmMem->tlb_habilitada && tlbHit != true){
				 //---------------------------------Logging Acceso TLB(miss)-----------------------------------------------//
				 t_tempLogueo* datosLogTLBmiss = agregar_reemplazarRegistroTLB(tlb,protInic->pid,protInic->paginas,frame);
				 loguearActualizacionTLB(logAdmMem,datosLogTLBmiss,frame,protInic->pid,protInic->paginas);
				 //--------------------------------------------------------------------------------------------------------//
			 }


			 contenido = memoriaPrincipal->Memoria[frame];

			 // Serializo el contenido del frame leido para su envío al CPU
			 tamanioContenido = configAdmMem->tamanio_marco;
			 void* bufferContAEnviar = malloc(sizeof(int)+(sizeof(char)*tamanioContenido));
			 memcpy(bufferContAEnviar,&tamanioContenido,sizeof(int));
			 int offset = sizeof(int);
			 memcpy(bufferContAEnviar+offset,contenido,tamanioContenido);
			 offset += tamanioContenido;
			 int enviado = send(socketCPU,bufferContAEnviar,offset,0);

			 printf("Se Enviaron %d Bytes. Tamanio Cont = %d , Cont = %s\n ",enviado,tamanioContenido,contenido);
			 free(bufferContAEnviar);
		 	 }
			 free(protInic);
	 }

void escrituraMemoria(int socketCPU, int socketSwap){

	t_protoc_escrituraProceso* pedido = malloc(sizeof(t_protoc_escrituraProceso));
	pedido->tipoInstrucc = ESCRIBIR;

	recv(socketCPU, &(pedido->pid),sizeof(int), 0);
	recv(socketCPU, &(pedido->pagina),sizeof(int), 0);
	recv(socketCPU, &(pedido->tamanio),sizeof(int), 0);
	pedido->contenido = malloc(pedido->tamanio);
	recv(socketCPU,pedido->contenido, pedido->tamanio,0);

	//---------Logging de solicitud de escritura---------//
	t_tempLogueo* tempLog = cargaDatosLogSolicEscr(pedido);
	loguearEvento(logAdmMem,tempLog);
	//---------------------------------------------------//

	// Búsqueda del frame asociado a la página del proceso en TLB
	int frame=-1;
	int marcosAsignados;
	int entradaTLB;
	int paginaReemp = 0;
	int tlbHit = false;
	char* contenido;

	char* pidBuscado = string_itoa(pedido->pid);
	t_tablaDePaginas* tablaPagsProceso;
	tablaPagsProceso = dictionary_get(tablasPags,pidBuscado);

	frame = configAdmMem->tlb_habilitada?buscarPaginaTLB(tlb,pedido->pid,pedido->pagina,&entradaTLB):frame;

	pthread_mutex_lock(&mutexTLB);
	referMem_TLB++; // Cantidad de referencias a memoria históricas (para calcular tasa de aciertos TLB)
	pthread_mutex_unlock(&mutexTLB);
		 if(frame != -1){

			 insertarPaginaenMP(pedido->contenido,memoriaPrincipal,&frame);
			 actualizarUtilizyModifPag(ESCRIBIR,tablaPagsProceso,pedido->pagina);

			 //----Logging Acceso TLB(hit) -------//
			 t_tempLogueo* datosLogTLB = cargaDatosAccesoTLB(pedido->pid,pedido->pagina,frame,entradaTLB);
			 datosLogTLB->hit = true;
			 tlbHit = datosLogTLB->hit;
			 loguearEvento(logAdmMem,datosLogTLB);
			 //-----------------------------------//

			 pthread_mutex_lock(&mutexTLB);
			 aciertos_TLB++; // Modificación de cantidad histórica de aciertos TLB
			 pthread_mutex_unlock(&mutexTLB);
		 }


		 if (frame == -1 && tlbHit != true)/*No se encontró página en TLB*/{

			 frame = buscarPaginaenMemoria(pedido->pid,pedido->pagina,tablasPags);
			 sleep(configAdmMem->retardo_memoria);

			 if(frame != -1){

				 insertarPaginaenMP(pedido->contenido,memoriaPrincipal,&frame);
				 actualizarUtilizyModifPag(ESCRIBIR,tablaPagsProceso,pedido->pagina);
				 //-----------Logging Acceso Memoria (hit)-----------------------------------------------//
				 t_tempLogueo* datosLogMemoria = cargaDatosAccesoMemoria(pedido->pid,pedido->pagina,frame);
				 datosLogMemoria->hit = true;
				 loguearEvento(logAdmMem,datosLogMemoria);
				 //--------------------------------------------------------------------------------------//
			 }
		 }

			 if(frame == -1 && tlbHit != true)/*No se encontró página en MP*/{

				 void* buffer = prepararSolicitudASwap(pedido);
				 contenido = solicitarContenidoASwap(socketSwap,buffer);

				 (tablaPagsProceso->contadorPF)++;
				 marcosAsignados = marcosUtilizadosProceso(tablaPagsProceso);
				 frame = buscarFrameLibre(memoriaPrincipal);

				 if (marcosAsignados == configAdmMem->max_marcos_proceso){
					 frame = reemplazarPagina(configAdmMem,socketSwap,memoriaPrincipal,tablaPagsProceso,&paginaReemp,pedido->pagina);
					 borrarRegistroTLBPagReemp(tlb,&paginaReemp,pedido->pid);
					 insertarPaginaenMP(pedido->contenido,memoriaPrincipal,&frame);
					 actualizarTablaPaginas(ESCRIBIR,frame,pedido->pagina,tablaPagsProceso);
					 sleep(configAdmMem->retardo_memoria);
					 loguearActualizacionMemoria(logAdmMem,pedido->pagina,paginaReemp,pedido->pid,frame);
				 }
				 else{
				 if (frame == -1){
					 if(marcosAsignados == 0) {
						finalizarProceso(memoriaPrincipal, dictionary_get(tablasPags, string_itoa(pedido->pid)));
						eliminarTablaDePaginasDelProceso(tablasPags,pedido->pid);
						t_protoc_Finaliza* protFinaliz = malloc(sizeof(t_protoc_Finaliza));
						protFinaliz->tipoInstrucc = FINALIZAR;
						protFinaliz->pid = pedido->pid;
						int rtaSwap = (int)notificarFinalizarSwap(socketSwap,protFinaliz);
						rtaSwap = -1;
						send(socketCPU,&rtaSwap,sizeof(int),0);
				 	 }
				 	 else {
				 		frame = reemplazarPagina(configAdmMem,socketSwap,memoriaPrincipal,tablaPagsProceso,&paginaReemp,pedido->pagina);
						borrarRegistroTLBPagReemp(tlb,&paginaReemp,pedido->pid);
						insertarPaginaenMP(pedido->contenido,memoriaPrincipal,&frame);
						actualizarTablaPaginas(ESCRIBIR,frame,pedido->pagina,tablaPagsProceso);
						sleep(configAdmMem->retardo_memoria);
						loguearActualizacionMemoria(logAdmMem,pedido->pagina,paginaReemp,pedido->pid,frame);
				 	 }
				 }
				 else {
					 insertarPaginaenMP(pedido->contenido,memoriaPrincipal,&frame);
					 actualizarTablaPaginas(ESCRIBIR,frame,pedido->pagina,tablaPagsProceso);
					 actualizarVectorClockModif(configAdmMem,tablaPagsProceso,pedido->pagina);
					 sleep(configAdmMem->retardo_memoria);
					 paginaReemp = -1;
					 loguearActualizacionMemoria(logAdmMem,pedido->pagina,paginaReemp,pedido->pid,frame);
				 	 }
				 }
			 }

		 (tablaPagsProceso->contadorPaginasAcc)++;

			 if (frame != -1){

				 if (configAdmMem->tlb_habilitada && tlbHit != true){
					 //---------------------------------Logging Acceso TLB(miss)-----------------------------------------------//
					 t_tempLogueo* datosLogTLBmiss = agregar_reemplazarRegistroTLB(tlb,pedido->pid,pedido->pagina,frame);
					 loguearActualizacionTLB(logAdmMem,datosLogTLBmiss,frame,pedido->pid,pedido->pagina);
					 //--------------------------------------------------------------------------------------------------------//
				 }

				 char respuesta = 1;

				 void* buffer = malloc(sizeof(char));
				 memcpy(buffer,&respuesta,sizeof(char));

				 send(socketCPU,buffer,sizeof(char),0);
				 printf("se envio confirmacion Escritura = %d\n",respuesta);

				 free(buffer);
			 	 }

				 free(pedido);

}

 void finalizacionProceso(int socketCpu,int socketSwap){

	 t_protoc_inicio_lectura_Proceso* pedidoCpu = malloc(sizeof(t_protoc_inicio_lectura_Proceso));


	 t_protoc_Finaliza* pedido = malloc(sizeof(t_protoc_Finaliza));


	 pedido->tipoInstrucc = FINALIZAR ;

	 recv(socketCpu,&pedidoCpu->paginas,sizeof(int),0);
	 recv(socketCpu,&pedidoCpu->pid,sizeof(int),0);

	 memcpy(&pedido->pid, &pedidoCpu->pid,sizeof(int));

	 t_tablaDePaginas* tablaDelProceso = dictionary_get(tablasPags, string_itoa(pedido->pid));

	 char respuestaSwap=1;


	 if(tablaDelProceso!=NULL )
	 {

	 finalizarProceso(memoriaPrincipal,tablaDelProceso);

	 /*Se muestra la cantidad de Fallos de Página y Paginas accedidas del proceso que finaliza*/
	 printf("Proceso Finalizado. \n");
	 printf("PID:%d \n",tablaDelProceso->pid);
	 printf("Cantidad total de páginas accedidas: %d \n",tablaDelProceso->contadorPaginasAcc);
	 printf("Cantidad de fallos de páginas producidos: %d \n",tablaDelProceso->contadorPF);
	 /*---------------------------------------------------------------------------------------*/

	 eliminarTablaDePaginasDelProceso(tablasPags,pedido->pid);
	 sleep(configAdmMem->retardo_memoria);


	 respuestaSwap = notificarFinalizarSwap(socketSwap,pedido);


	 }



	 if(respuestaSwap)
	 {

		notificarFinalizarCpu(socketCpu);

	 }




	 free(pedido);
	 free(pedidoCpu);


 }


 char notificarFinalizarSwap(int socket, t_protoc_Finaliza* pedido)
 {
	 void*buffer = malloc(sizeof(t_protoc_Finaliza));


	 memcpy(buffer,pedido,sizeof(t_protoc_Finaliza));

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

 void crear_e_insertar_TabladePaginas(int paginas, int pid, t_dictionary* tablasPagsProcesos) {

	 int i;

	 t_tablaDePaginas* tablaPaginasProceso = malloc(sizeof(t_tablaDePaginas));
	 tablaPaginasProceso->Pagina = (t_regPagina**)calloc(paginas,sizeof(t_regPagina*));
	 tablaPaginasProceso->pid = pid;
	 tablaPaginasProceso->cantTotalPaginas = paginas;
	 tablaPaginasProceso->contadorPF = 0;
	 tablaPaginasProceso->contadorPaginasAcc = 0;

	 for (i=0; i<paginas; ++i){
		 tablaPaginasProceso->Pagina[i] = malloc(sizeof(t_regPagina));
		 tablaPaginasProceso->Pagina[i]->idFrame = -1;
		 tablaPaginasProceso->Pagina[i]->bitPresencia = 0;
		 tablaPaginasProceso->Pagina[i]->bitModificado = 0;
		 tablaPaginasProceso->Pagina[i]->bitUtilizado = 0;
		 tablaPaginasProceso->Pagina[i]->horaIngreso = MAXTIME;
		 tablaPaginasProceso->Pagina[i]->horaUtilizacion = MAXTIME;//Espacio que ocupa la hora en formato 'hh:mm:ss:mmmm'
	 };

	 if (strcmp(configAdmMem->algoritmo_reemplazo,"CLOCKMODIF") == 0){
			 tablaPaginasProceso->vectClockModif = calloc(configAdmMem->max_marcos_proceso,sizeof(int));
			 for (i = 0; i < configAdmMem->max_marcos_proceso; ++i) {
				tablaPaginasProceso->vectClockModif[i] = malloc(sizeof(int));
				*(tablaPaginasProceso->vectClockModif[i]) = -1;
			 }
			 tablaPaginasProceso->posicClockModif = -1;
		 }

	 char* pidConv = malloc(20);
	 pidConv = string_itoa(pid);
	 dictionary_put(tablasPagsProcesos,pidConv,tablaPaginasProceso);

	 free(pidConv);
	 //free(tablaPaginasProceso);
 }

 void* deserializarInfoCPU_Inicio_Lectura(int socketCPU, t_protoc_inicio_lectura_Proceso* protInic_Lect, char InstrSent){

	 void* buffer = malloc(sizeof(t_protoc_inicio_lectura_Proceso));

	 protInic_Lect->tipoInstrucc = InstrSent;

 	 recv(socketCPU,&(protInic_Lect->paginas),sizeof(int),0);
 	 recv(socketCPU,&(protInic_Lect->pid),sizeof(int),0);

 	 memcpy(buffer,&(protInic_Lect->tipoInstrucc),sizeof(char));
 	 int offset = sizeof(char);
 	 memcpy((buffer+offset),&protInic_Lect->paginas,sizeof(int));
 	 offset += sizeof(int);
 	 memcpy((buffer+offset),&protInic_Lect->pid,sizeof(int));

 	 return buffer;
 }

 char* solicitarContenidoASwap(int socketSwap,void* buffer){
 	 int tamanioContenido;
 	 char* contenido;
 	 send(socketSwap,buffer,sizeof(char)+(sizeof(int)*2),0);
 	 recv(socketSwap,&tamanioContenido,sizeof(int),0);
 	 contenido = malloc(tamanioContenido);
 	 recv(socketSwap,contenido,tamanioContenido,0);
 	 return contenido;
 }

 void* prepararSolicitudASwap(t_protoc_escrituraProceso* pedido){

	 char instruccSwap = LEER;
	 void* buffer = malloc(sizeof(char)+(sizeof(int)*2));
	 int offset = 0;

	 memcpy(buffer,&instruccSwap,sizeof(char));
	 offset++;
	 memcpy(buffer+offset,&(pedido->pagina),sizeof(int));
	 offset += sizeof(int);
	 memcpy(buffer+offset,&(pedido->pid),sizeof(int));
	 return buffer;

 }



 void eliminarTablaDePaginasDelProceso(t_dictionary* tablasPags,int pid)
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



void tlbFlush(){

	pthread_mutex_lock(&mutex);
	tlb_Flush(tlb);
	pthread_mutex_unlock(&mutex);
	loggingSenial(logAdmMem,SIGUSR1_);
	pthread_exit(EXIT_SUCCESS);

}

void tlbFlushHandler(int signum) {

	pthread_create(&hilo_tlbFlush,&attr,(void*)tlbFlush,NULL);
	signal(SIGUSR1,tlbFlushHandler);
	return;

}


void memFlush() {

			pthread_mutex_lock(&mutex);
			tlb_Flush(tlb);
			mem_Flush(memoriaPrincipal,tablasPags);
			pthread_mutex_unlock(&mutex);
			loggingSenial(logAdmMem,SIGUSR2_);
			pthread_exit(EXIT_SUCCESS);
}





void memoriaFlushHandler(int signum) {

	pthread_create(&hilo_memFlush,&attr,(void*)memFlush,NULL);
	signal(SIGUSR2,memoriaFlushHandler);

	return;

	}

void dumpMemoriaHandler(int signum)
{
	 pid = fork();

	 if(pid!=0)
	 {
		 signal(SIGPOLL,dumpMemoriaHandler);
		 wait(NULL);
		/// waitpid(pid,NULL,WNOHANG);

	 }
	 else
	 {
		 memoryDump();
		 exit(0);

	 }

}




void memoryDump()
{

   int i ;

    loggingSenial(logAdmMem,SIGPOLL_);
	log_info(logAdmMem,"MEMORY DUMP MARCOS = %d ",memoriaPrincipal->cantMarcos);
	for(i=0; i<memoriaPrincipal->cantMarcos; i++)
	{
		char *frame = malloc(configAdmMem->tamanio_marco) ;

		strcpy(frame,memoriaPrincipal->Memoria[i]);

		log_info(logAdmMem, "FRAME #%d || CONTENIDO = %s",i,frame);

		free(frame);

	}
}

void publicarTasaAciertos(){

float tasaAciertos;

while(1)
{
	sleep(20);

	pthread_mutex_lock(&mutexTLB);
	tasaAciertos = (referMem_TLB != 0.00)?aciertos_TLB/referMem_TLB:0.00;
	pthread_mutex_unlock(&mutexTLB);

	printf("Tasa de aciertos histórica de TLB = %.2f \n",tasaAciertos);
}

}
