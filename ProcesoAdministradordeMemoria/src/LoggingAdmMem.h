/*
 * LoggingAdmMem.h
 *
 *  Created on: 12/11/2015
 *      Author: utnso
 */

#ifndef LOGGINGADMMEM_H_
#define LOGGINGADMMEM_H_

#include <stdbool.h>
#include "ProtocsyFuncsRecvMsjs.h"
#include <commons/log.h>

typedef enum {
	CREACIONMPROC,
	SOLICITUD_LECTURA,
	SOLICITUD_ESCRITURA,
	ACCESO_TLB,
	ACCESO_MEMORIA,
	STATUSSENIAL,
} t_Evento;

typedef struct{
	int pid;
	int paginas;
	int frame;
	int entradaTLB;
	bool regSaliente;
	bool hit;
	char* algoritmo_reemplazo;
	t_Evento tipoEvento;
}t_tempLogueo;


void loguearEvento(t_log* logAdmMem,t_tempLogueo* estructDatosALoguear);
void loguearCreacionMProc(t_log* logAdmMem,t_tempLogueo* estructDatosALoguear);
void loguearSolicitudLect(t_log* logAdmMem,t_tempLogueo* estructDatosALoguear);
void loguearSolicitudEscr(t_log* logAdmMem,t_tempLogueo* estructDatosALoguear);
void loguearAccesoTLB(t_log*logAdmMem,t_tempLogueo* estructDatosALoguear);
void loguearActualizacionTLB(t_log*logAdmMem,t_tempLogueo* datosLogTLB,int frame,int pid,int pagina);
void loguearAccesoMemoria(t_log*logAdmMem,t_tempLogueo* estructDatosALoguear);
void loguearActualizacionMemoria(t_log* logAdmMem,int pagina,int paginaReemp, int pid,int frame);
void loguearSenial(t_log*logAdmMem);
t_tempLogueo* cargaDatosLogCrearMProc (t_protoc_inicio_lectura_Proceso* arg);
t_tempLogueo* cargaDatosLogSolicLect (t_protoc_inicio_lectura_Proceso* arg);
t_tempLogueo* cargaDatosLogSolicEscr (t_protoc_escrituraProceso* arg);
t_tempLogueo* cargaDatosAccesoTLB (int pid, int pagina, int frame, int entradaTLB);
t_tempLogueo* cargaDatosAccesoMemoria (int pid, int pagina, int frame);


#endif /* LOGGINGADMMEM_H_ */
