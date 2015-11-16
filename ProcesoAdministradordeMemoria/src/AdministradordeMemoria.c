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
#include "LoggingAdmMem.h"

#define SI "SI"
/*
typedef struct{
	int idFrame;
	char bitPresencia;
	char bitModificado;
}t_regTablaPaginas; */

void crear_e_insertar_TabladePaginas(int, int, t_dictionary*);
void* deserializarInfoCPU_Inicio_Lectura(int, t_protoc_inicio_lectura_Proceso*,char);
void lecturaMemoria(int,int,MEMORIAPRINCIPAL*,TLB*,t_dictionary*);
void escrituraMemoria(int,int);
void finalizacionProceso(int socketCpu,int socketSwap);
char notificarFinalizarSwap(int socket, t_protoc_Finaliza* pedido);
void notificarFinalizarCpu( int socket);
void elminarTablaDePaginasDelProceso(t_dictionary*  tablasPags, int pid);
char escribirContenido(t_protoc_escrituraProceso* pedido, int socketSwap, int* cantIter);
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
	 	 	 	 logAdmMem = log_create("AdmMemoria.log", "Administrador de memoria",false, LOG_LEVEL_INFO);

	 	 	 	 /* Inicialización de espacio de memoria, array indicador de memoria libre y TLB */

	 	 	 	 t_memoria_crear(&memoriaPrincipal,configAdmMem);


	 	 	 	 if (configAdmMem->tlb_habilitada) {
	 	 	        tlb = t_tlb_crear(configAdmMem);
	 	 	 	 }

	 	 	 	 /* Se crea una estructura dinámica que contendrá las tablas de páginas de los procesos en ejecución*/

	 	 	 	 tablasPags = malloc(sizeof(t_dictionary*));
	 	 	 	 tablasPags = dictionary_create();

	 	 	 	 /*Conexión del administrador de memoria como cliente al Swap y como Servidor con CPU*/

	 	 	 	 socketSwap = conectar_cliente(configAdmMem->puerto_swap,configAdmMem->ip_swap);

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
 void lecturaMemoria(int socketCPU, int socketSwap, MEMORIAPRINCIPAL* memPrincip, TLB* tlb, t_dictionary* tablasdePagsProcesos){

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


	 configAdmMem->tlb_habilitada?:(frame = buscarPaginaTLB(tlb,protInic->pid,protInic->paginas,&entradaTLB));

	 //----Logging Acceso TLB(hit) -------//
	 if(frame != -1){
		 t_tempLogueo* datosLogTLB = cargaDatosAccesoTLB(protInic->pid,protInic->paginas,frame,entradaTLB);
		 loguearEvento(logAdmMem,datosLogTLB);
	 }
	 //-----------------------------------//

	 if (frame == -1)/*No se encontró página en TLB*/{

		 frame = buscarPaginaenMemoria(protInic->pid,protInic->paginas,tablasdePagsProcesos);
		 sleep(configAdmMem->retardo_memoria);
	 }

		 //-----------Logging Acceso Memoria (hit)--------//
		 if(frame != -1){
			 t_tempLogueo* datosLogMemoria = cargaDatosAccesoMemoria(protInic->pid,protInic->paginas,frame);
			 datosLogMemoria->hit = true;
			 loguearEvento(logAdmMem,datosLogMemoria);
		 }
		 //-----------------------------------------------//

		 char* pidBuscado = string_itoa(protInic->pid);
		 t_tablaDePaginas* tablaPagsProceso;
		 tablaPagsProceso = dictionary_get(tablasdePagsProcesos,pidBuscado);

		 //enviarSolicitudSwap() y recibirContenido de Swap;
		 send(socketSwap,buffer,sizeof(char)+(sizeof(int)*2),0);
		 recv(socketSwap,&tamanioContenido,sizeof(int),0);
		 contenido = malloc(tamanioContenido);
		 recv(socketSwap,contenido,tamanioContenido,0);

		 if(frame == -1)/*No se encontró página en MP*/{

			 marcosAsignados = marcosUtilizadosProceso(tablaPagsProceso);
			 frame = buscarFrameLibre(memPrincip);

			 if (marcosAsignados == configAdmMem->max_marcos_proceso){
				 frame = reemplazarPaginaFIFO(socketSwap,contenido,memPrincip,tablaPagsProceso,&paginaReemp);
				 insertarPaginaenMP(contenido,memPrincip,&frame);
				 actualizarTablaPaginas(frame,protInic->paginas,tablaPagsProceso);
				 sleep(configAdmMem->retardo_memoria);
				 loguearActualizacionMemoria(logAdmMem,protInic->paginas,paginaReemp,protInic->pid,frame);
			 }
			 if (frame == -1){
				 if(marcosAsignados == 0) {
					finalizarProceso(&memoriaPrincipal, dictionary_get(tablasPags, string_itoa(protInic->pid)));
					elminarTablaDePaginasDelProceso(tablasPags,protInic->pid);
					t_protoc_Finaliza* protFinaliz = malloc(sizeof(t_protoc_Finaliza));
					protFinaliz->tipoInstrucc = FINALIZAR;
					protFinaliz->pid = protInic->pid;
					int rtaSwap = (int)notificarFinalizarSwap(socketSwap,protFinaliz);
					rtaSwap = -1;
					send(socketCPU,&rtaSwap,sizeof(int),0);
			 	 }
			 	 else {
					frame = reemplazarPaginaFIFO(socketSwap,contenido,memPrincip,tablaPagsProceso,&paginaReemp);
					insertarPaginaenMP(contenido,memPrincip,&frame);
					actualizarTablaPaginas(frame,protInic->paginas,tablaPagsProceso);
					sleep(configAdmMem->retardo_memoria);
					loguearActualizacionMemoria(logAdmMem,protInic->paginas,paginaReemp,protInic->pid,frame);
			 	 }
			 }
			 else {

				 insertarPaginaenMP(contenido,memPrincip,&frame);
				 actualizarTablaPaginas(frame,protInic->paginas,tablaPagsProceso);
				 sleep(configAdmMem->retardo_memoria);
				 paginaReemp = -1;
				 loguearActualizacionMemoria(logAdmMem,protInic->paginas,paginaReemp,protInic->pid,frame);
			 	 }
		 }

		 // if (configAdmMem->tlb_habilitada){
		 //---------------------------------Logging Acceso TLB(miss)-----------------------------------------------//
		 // t_tempLogueo* datosLogTLBmiss = agregar_reemplazarRegistroTLB(tlb,protInic->pid,protInic->paginas,frame);
		 // loguearActualizacionTLB(logAdmMem,datosLogTLBmiss,protInic->pid,protInic->paginas);
		 //--------------------------------------------------------------------------------------------------------//
		 // }

			 contenido = memoriaPrincipal.Memoria[frame];

			 // Serializo el contenido del frame leido para su envío al CPU
			 tamanioContenido = configAdmMem->tamanio_marco;
			 void* bufferContAEnviar = malloc(sizeof(int)+(sizeof(char)*tamanioContenido));
			 memcpy(bufferContAEnviar,&tamanioContenido,sizeof(int));
			 int offset = sizeof(int);
			 memcpy(bufferContAEnviar+offset,contenido,tamanioContenido);
			 offset += tamanioContenido;
			 int enviado = send(socketCPU,bufferContAEnviar,offset,0);

			 printf("Se Enviaron %d Bytes. Tamanio Cont = %d , Cont = %s\n ",enviado,tamanioContenido,contenido);

			 free(protInic);
			 free(bufferContAEnviar);
			 free(contenido);

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
	 int contIter = 0;
	 char respuesta =  escribirContenido(pedido, socketSwap,&contIter);

	 void* buffer = malloc(sizeof(char));
	 memcpy(buffer,&respuesta,sizeof(char));

	 send(socketCPU,buffer,sizeof(char),0);
	 printf("se envio confirmacion Escritura = %d\n",respuesta);

	 free(buffer);
	 free(pedido);


 }


 char escribirContenido(t_protoc_escrituraProceso* pedido, int socketSwap, int* cantIter)
 {	int frame =-1;
 	int entradaTLB;

 	t_tablaDePaginas* tablaDePaginas =   dictionary_get(tablasPags,string_itoa(pedido->pid));

 	 if(configAdmMem->tlb_habilitada)
 		 frame = buscarPaginaTLB(tlb,pedido->pid,pedido->pagina,&entradaTLB);

 	 //----------------Logging Acceso TLB (hit)--------//
 	 if(frame != -1){
 		 t_tempLogueo* datosLogTLB = cargaDatosAccesoTLB(pedido->pid,pedido->pagina,frame,entradaTLB);
 		 loguearEvento(logAdmMem,datosLogTLB);
 	 }
 	 //------------------------------------------------//

	  frame = (frame <0)?buscarPaginaenMemoria(pedido->pid, pedido->pagina,tablasPags):frame;
	  sleep(configAdmMem->retardo_memoria);


	 if(frame>=0)
	 {
		 memoriaPrincipal.Memoria[frame] = pedido->contenido;
		 memoriaPrincipal.MemoriaLibre[frame]= 1;
		 tablaDePaginas->Pagina[pedido->pagina]->bitPresencia=1;
		 tablaDePaginas->Pagina[pedido->pagina]->bitModificado=1;

		 if (*cantIter){
			 //--------------------Logging Acceso Memoria (Page Fault)------------------------------//
			 t_tempLogueo* datosPagAgrMem = cargaDatosAccesoMemoria(pedido->pid,pedido->pagina,frame);
			 datosPagAgrMem->hit = false;
			 loguearEvento(logAdmMem,datosPagAgrMem);
			 //-------------------------------------------------------------------------------------//
		 }
		 else {
			 //------------------------Logging Acceso Memoria (hit)-----------------------------------------------//
			 t_tempLogueo* datosLogMemoria = cargaDatosAccesoMemoria(pedido->pid,pedido->pagina,frame);
			 datosLogMemoria->hit = true;
			 loguearEvento(logAdmMem,datosLogMemoria);
			 //--------------------------------------------------------------------------------------//
		 }
		 return 1 ;

	 }else
	 {
		 if(! t_hayFrameLibre(&memoriaPrincipal, tablaDePaginas,configAdmMem->max_marcos_proceso))
		 {
			/*
			 * en este punto, quizas tendria sentido que el cpu sepa el motivo de porque se rechazo la lectura.
			 * Sea por la necesidad de reemplazar.
			 * O porque la memoria esta llena.
			 */
			 return 0;

		 }
		 solicitarPagina(pedido,socketSwap);
		 *cantIter=1;
		 return escribirContenido( pedido, socketSwap,cantIter);


	 }



 }


 void solicitarPagina(t_protoc_escrituraProceso* pedido, int socketSwap)
 {
	 t_protoc_inicio_lectura_Proceso* pedidoLectura = malloc(sizeof(t_protoc_inicio_lectura_Proceso));
	 pedidoLectura->pid  = pedido->pid;
	 pedidoLectura->paginas = pedido->pagina;
	 pedidoLectura->tipoInstrucc = LEER;

	int enviados= send(socketSwap,&pedidoLectura->tipoInstrucc,sizeof(char),0);
	enviados+= send(socketSwap,&pedidoLectura->paginas,sizeof(int),0);
	enviados+=send(socketSwap,&pedidoLectura->pid,sizeof(int),0);

	 printf("Se solicito pagina a Swap. BytesEnviados =%d , PID= %d, PAGINA = %d \n",enviados, pedidoLectura->pid, pedidoLectura->paginas);

/*
	 void* buffer = malloc(sizeof(t_protoc_inicio_lectura_Proceso));
	 int offset = 0 ;

	 memcpy(buffer,&(pedidoLectura->tipoInstrucc),sizeof(char));
	 offset+=sizeof(int);
	 memcpy(buffer+offset,&(pedidoLectura->paginas),sizeof(int));
	 offset+=sizeof(int);
	 memcpy(buffer+offset,&(pedidoLectura->pid),sizeof(int));


	 send(socketSwap,buffer,sizeof(t_protoc_inicio_lectura_Proceso),0);
	 free(buffer);
*/
	 int tamanioContenido;
	 recv(socketSwap,&tamanioContenido,sizeof(int),0);

	 char* contenido = malloc(tamanioContenido);
	 recv(socketSwap,contenido,tamanioContenido,0);



	int frame = t_cargarContenido(&memoriaPrincipal,contenido);

	/*actualizar tabla de paginas paginas */
	t_tablaDePaginas* tabla =  dictionary_get(tablasPags,string_itoa(pedido->pid));

	tabla->Pagina[pedido->pagina]->bitPresencia=1;
	tabla->Pagina[pedido->pagina]->idFrame=frame;
	tabla->Pagina[pedido->pagina]->horaIngreso=90000000;


	//if(configAdmMem->tlb_habilitada)
	//agregar_reemplazarRegistroTLB(tlb,pedido->pid,pedido->pagina,frame);
 }



 void finalizacionProceso(int socketCpu,int socketSwap){

	 t_protoc_inicio_lectura_Proceso* pedidoCpu = malloc(sizeof(t_protoc_inicio_lectura_Proceso));


	 t_protoc_Finaliza* pedido = malloc(sizeof(t_protoc_Finaliza));


	 pedido->tipoInstrucc = FINALIZAR ;

	 recv(socketCpu,&pedidoCpu->paginas,sizeof(int),0);
	 recv(socketCpu,&pedidoCpu->pid,sizeof(int),0);

	 memcpy(&pedido->pid, &pedidoCpu->pid,sizeof(int));


	 if (configAdmMem->tlb_habilitada)
		 t_tlb_limpiar(tlb, pedido->pid);


	 finalizarProceso(&memoriaPrincipal, dictionary_get(tablasPags, string_itoa(pedido->pid)));
	 elminarTablaDePaginasDelProceso(tablasPags,pedido->pid);
	 sleep(configAdmMem->retardo_memoria);


	 char respuestaSwap = notificarFinalizarSwap(socketSwap,pedido);

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



 void crear_e_insertar_TabladePaginas(int paginas, int pid, t_dictionary* tablasPagsProcesos) {
	 //t_tablaDePaginas* tablaPaginasProceso = malloc(sizeof(t_regPagina**)+(sizeof(int)*2));
	 //tablaPaginasProceso->Pagina = calloc(paginas,sizeof(t_regPagina));
	 t_tablaDePaginas* tablaPaginasProceso = malloc(sizeof(t_regPagina**)+(sizeof(int)*2));
	 tablaPaginasProceso->Pagina = (t_regPagina**)calloc(paginas,sizeof(t_regPagina*));
	 tablaPaginasProceso->pid = pid;
	 tablaPaginasProceso->cantTotalPaginas = paginas;

	 int i;
	 for (i=0; i<paginas; ++i){
		 tablaPaginasProceso->Pagina[i] = malloc(sizeof(t_regPagina));
		 tablaPaginasProceso->Pagina[i]->idFrame = -1;
		 tablaPaginasProceso->Pagina[i]->bitPresencia = 0;
		 tablaPaginasProceso->Pagina[i]->bitModificado = 0;
		 tablaPaginasProceso->Pagina[i]->horaIngreso = 90000000;//Espacio que ocupa la hora en formato 'hh:mm:ss:mmmm'
	 };

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
