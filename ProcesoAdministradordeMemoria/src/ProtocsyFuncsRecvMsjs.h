/*
 * ProtocsyFuncsRecvMsjs.h
 *
 *  Created on: 19/10/2015
 *      Author: utnso
 */

#ifndef PROTOCSYFUNCSRECVMSJS_H_
#define PROTOCSYFUNCSRECVMSJS_H_

enum {INICIAR=1, LEER , ESCRIBIR , FINALIZAR};

typedef struct {
	char tipoInstrucc;
	int paginas;
	int pid;
}__attribute__((packed))t_protoc_inicio_lectura_Proceso;



typedef struct {
	char tipoInstrucc;
	int pid;
}__attribute__((packed))t_protoc_Finaliza;


typedef struct {
	char tipoInstrucc;
	int pid;
	int pagina;
	int tamanio;
	char* contenido;

}__attribute__((packed))t_protoc_escrituraProceso;

typedef struct {
	int tamanio;
	char* contenido;

}__attribute__((packed))t_protoc_lectura_Swap;





#endif /* PROTOCSYFUNCSRECVMSJS_H_ */
