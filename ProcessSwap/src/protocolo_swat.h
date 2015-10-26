/*
 * protocolo_swat.h
 *
 *  Created on: 25/10/2015
 *      Author: utnso
 */

#ifndef PROTOCOLO_SWAT_H_
#define PROTOCOLO_SWAT_H_

typedef struct protocolo_cpu_memoria
{
	char tipo_Instruccion;
	int pid;
	int paginas;

}__attribute__((packed)) t_prot_cpu_mem ;

#endif /* PROTOCOLO_SWAT_H_ */
