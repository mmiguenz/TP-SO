/*
 * ProtocsyFuncsRecvMsjs.h
 *
 *  Created on: 19/10/2015
 *      Author: utnso
 */

#ifndef PROTOCSYFUNCSRECVMSJS_H_
#define PROTOCSYFUNCSRECVMSJS_H_

typedef enum {INICIAR=1, LEER , ESCRIBIR , FINALIZAR};

typedef struct {
	char tipoInstrucc;
	int paginas;
	int pid;
}__attribute__((packed))t_protoc_inicio_lectura_Finaliza_Proceso;

typedef struct {
	char tipoInstrucc;
	int pagina;
	char* contenido;
	int pid;
}__attribute__((packed))t_protoc_escrituraProceso;




#endif /* PROTOCSYFUNCSRECVMSJS_H_ */
