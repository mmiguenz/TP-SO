/*
 * ProtocsyFuncsRecvMsjs.h
 *
 *  Created on: 19/10/2015
 *      Author: utnso
 */

#ifndef PROTOCSYFUNCSRECVMSJS_H_
#define PROTOCSYFUNCSRECVMSJS_H_

typedef struct {
	char tipoInstrucc;
	int paginas;
	int PID;
}__attribute__((packed))t_protoc_inicio_lectura_Proceso;

typedef struct {
	char tipoInstrucc;
	int pagina;
	char* contenido;
	int PID;
}__attribute__((packed))t_protoc_inicioProceso;




#endif /* PROTOCSYFUNCSRECVMSJS_H_ */
