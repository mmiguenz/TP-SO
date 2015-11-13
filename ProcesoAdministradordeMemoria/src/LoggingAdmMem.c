/*
 * LoggingAdmMem.c
 *
 *  Created on: 12/11/2015
 *      Author: utnso
 */


#include <commons/log.h>
#include <stdlib.h>
#include "LoggingAdmMem.h"
#include "ProtocsyFuncsRecvMsjs.h"


void loguearEvento(t_log* logAdmMem,t_tempLogueo* estructDatosALoguear)
{
	switch (estructDatosALoguear->tipoEvento) {

		case CREACIONMPROC:
		loguearCreacionMProc(logAdmMem,estructDatosALoguear);
		break;
		case SOLICITUD_LECTURA:
		loguearSolicitudLect(logAdmMem,estructDatosALoguear);
		break;
		case SOLICITUD_ESCRITURA:
		loguearSolicitudEscr(logAdmMem,estructDatosALoguear);
		break;
		case ACCESO_TLB:
		loguearAccesoTLB(logAdmMem,estructDatosALoguear);
		break;
		case ACCESO_MEMORIA:
		loguearAccesoMemoria(logAdmMem,estructDatosALoguear);
		break;
		case STATUSSENIAL:
		loguearSenial(logAdmMem);
		break;

	}
}

void loguearCreacionMProc(t_log* logAdmMem,t_tempLogueo* estructDatosALoguear){

	char* lineaALoguear = malloc(120);
	sprintf(lineaALoguear,"CREACION DE MPROC || PID:%i || Cantidad de paginas asignadas:%i",estructDatosALoguear->pid,estructDatosALoguear->paginas);
	log_info(logAdmMem,lineaALoguear);
	free(lineaALoguear);
	free(estructDatosALoguear);

}

void loguearSolicitudLect(t_log* logAdmMem,t_tempLogueo* estructDatosALoguear){

	char* lineaALoguear = malloc(120);
	sprintf(lineaALoguear,"SOLICITUD DE LECTURA RECIBIDA || PID: %i || Página solicitada: %i",estructDatosALoguear->pid,estructDatosALoguear->paginas);
	log_info(logAdmMem,lineaALoguear);
	free(lineaALoguear);
	free(estructDatosALoguear);
}

void loguearSolicitudEscr(t_log* logAdmMem,t_tempLogueo* estructDatosALoguear){

	char* lineaALoguear = malloc(120);
	sprintf(lineaALoguear,"SOLICITUD DE ESCRITURA RECIBIDA || PID: %i || Página a escribir: %i",estructDatosALoguear->pid,estructDatosALoguear->paginas);
	log_info(logAdmMem,lineaALoguear);
	free(lineaALoguear);
	free(estructDatosALoguear);

}

void loguearAccesoTLB(t_log*logAdmMem,t_tempLogueo* estructDatosALoguear){
	char* lineaALoguear = malloc(120);
	if (estructDatosALoguear->hit == true){

		sprintf(lineaALoguear,"ACCESO TLB - PAGINA ENCONTRADA (TLB HIT) || Entrada_TLB: %i || PID: %i || Página: %i || Frame: %i",estructDatosALoguear->pid,estructDatosALoguear->paginas,estructDatosALoguear->entradaTLB,estructDatosALoguear->frame);
		log_info(logAdmMem,lineaALoguear);
		free(lineaALoguear);
		free(estructDatosALoguear);
	}
	else {
		if (estructDatosALoguear->regSaliente){
			sprintf(lineaALoguear,"ACCESO TLB - REEMPLAZO (TLB MISS) - PAGINA REEMPLAZADA || #Entrada_TLB: %i || PID: %i || Página: %i || Frame: %i",estructDatosALoguear->pid,estructDatosALoguear->paginas,estructDatosALoguear->entradaTLB,estructDatosALoguear->frame);
			log_info(logAdmMem,lineaALoguear);
			free(lineaALoguear);
			free(estructDatosALoguear);
		}
		else{
			sprintf(lineaALoguear,"ACCESO TLB - REEMPLAZO (TLB MISS) - PAGINA INCORPORADA || #Entrada_TLB: %i || PID: %i || Página: %i || Frame: %i",estructDatosALoguear->pid,estructDatosALoguear->paginas,estructDatosALoguear->entradaTLB,estructDatosALoguear->frame);
			log_info(logAdmMem,lineaALoguear);
			free(lineaALoguear);
			free(estructDatosALoguear);
		}

	}
}

void loguearActualizacionTLB(t_log* logAdmMem, t_tempLogueo* datosLogTLB,int pid,int pagina){
	datosLogTLB->tipoEvento = ACCESO_TLB;
	if(datosLogTLB->regSaliente == false){
		datosLogTLB->tipoEvento = ACCESO_TLB;
		datosLogTLB->hit = false;
		loguearEvento(logAdmMem,datosLogTLB);//Logging por un registro agregado a TLB
	}
	else{
		t_tempLogueo* datosRegEntranteTLB = cargaDatosAccesoTLB(pid,pagina,datosLogTLB->frame,datosLogTLB->entradaTLB);
		datosRegEntranteTLB->tipoEvento = ACCESO_TLB;
		datosRegEntranteTLB->hit = false;
		datosRegEntranteTLB->regSaliente = false;
		loguearEvento(logAdmMem,datosRegEntranteTLB);//Logging registro entrante a TLB
		loguearEvento(logAdmMem,datosLogTLB);//Logging registro saliente de TLB
	}
}
void loguearAccesoMemoria(t_log*logAdmMem,t_tempLogueo* estructDatosALoguear){
	char* lineaALoguear = malloc(120);
	if (estructDatosALoguear->hit == true){

		sprintf(lineaALoguear,"ACCESO MEMORIA - PAGINA ENCONTRADA (HIT) || PID: %i || Página: %i || Frame: %i",estructDatosALoguear->pid,estructDatosALoguear->paginas,estructDatosALoguear->frame);
		log_info(logAdmMem,lineaALoguear);
		free(lineaALoguear);
		free(estructDatosALoguear);
	}
	else{ //Aca va a haber que poner un switch para informar el estado de las colas cuando se incorporen los algoritmos LRU y clock modificado
		if (estructDatosALoguear->regSaliente){

			sprintf(lineaALoguear,"ACCESO MEMORIA - PAGE FAULT - PAGINA REEMPLAZADA || Frame: %i || PID: %i || Página: %i",estructDatosALoguear->frame,estructDatosALoguear->pid,estructDatosALoguear->paginas);
			log_info(logAdmMem,lineaALoguear);
			free(lineaALoguear);
			free(estructDatosALoguear);

			}
			else{

			sprintf(lineaALoguear,"ACCESO MEMORIA - PAGE FAULT - PAGINA INGRESADA || Frame: %i || PID: %i || Página: %i",estructDatosALoguear->frame,estructDatosALoguear->pid,estructDatosALoguear->paginas);
			log_info(logAdmMem,lineaALoguear);
			free(lineaALoguear);
			free(estructDatosALoguear);
			}
	}

}

void loguearActualizacionMemoria(t_log* logAdmMem,int pagina,int paginaReemp,int pid, int frame){
t_tempLogueo* datosLogMemPF;
	if(paginaReemp != -1){
		t_tempLogueo* datosLogMemPF2;
		datosLogMemPF = cargaDatosAccesoMemoria(pid,pagina,frame);
		datosLogMemPF->hit = false;
		loguearEvento(logAdmMem,datosLogMemPF);

		datosLogMemPF2 = cargaDatosAccesoMemoria(pid,paginaReemp,frame);
		datosLogMemPF2->regSaliente = true;
		datosLogMemPF-> hit = false;
		loguearEvento(logAdmMem,datosLogMemPF2);
}
	else{
		datosLogMemPF = cargaDatosAccesoMemoria(pid,pagina,frame);
		datosLogMemPF->hit = false;
		loguearEvento(logAdmMem,datosLogMemPF);
}
}

void loguearSenial(t_log*logAdmMem){

}

t_tempLogueo* cargaDatosLogCrearMProc (t_protoc_inicio_lectura_Proceso* arg){

	t_tempLogueo* tempLog = malloc(sizeof(t_tempLogueo));
	tempLog->tipoEvento = CREACIONMPROC;
	tempLog->pid = arg->pid;
	tempLog->paginas = arg->paginas;
	return tempLog;
 }

t_tempLogueo* cargaDatosLogSolicLect (t_protoc_inicio_lectura_Proceso* arg){

	t_tempLogueo* tempLog = malloc(sizeof(t_tempLogueo));
	tempLog->tipoEvento = SOLICITUD_LECTURA;
	tempLog->pid = arg->pid;
	tempLog->paginas = arg->paginas;
	return tempLog;
 }

t_tempLogueo* cargaDatosLogSolicEscr (t_protoc_escrituraProceso* arg){

	t_tempLogueo* tempLog = malloc(sizeof(t_tempLogueo));
	tempLog->tipoEvento = SOLICITUD_ESCRITURA;
	tempLog->pid = arg->pid;
	tempLog->paginas = arg->pagina;
	return tempLog;
 }

t_tempLogueo* cargaDatosAccesoTLB (int pid, int pagina, int frame, int entradaTLB){

	t_tempLogueo* tempLog = malloc(sizeof(t_tempLogueo));
	tempLog->tipoEvento = ACCESO_TLB;
	tempLog->regSaliente = false;
	tempLog->pid = pid;
	tempLog->paginas = pagina;
	tempLog->frame = frame;
	tempLog->entradaTLB = entradaTLB;
	return tempLog;
}

t_tempLogueo* cargaDatosAccesoMemoria (int pid, int pagina, int frame){

	t_tempLogueo* tempLog = malloc(sizeof(t_tempLogueo));
	tempLog->tipoEvento = ACCESO_MEMORIA;
	tempLog->regSaliente = false;
	tempLog->pid = pid;
	tempLog->paginas = pagina;
	tempLog->frame = frame;
	return tempLog;
}
