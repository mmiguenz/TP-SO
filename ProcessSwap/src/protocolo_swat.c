/*
 * protocolo_swat.c
 *
 *  Created on: 25/10/2015
 *      Author: utnso
 */


typedef struct protocolo_cpu_memoria
{
	char tipo_Instruccion;
	int pid;
	int paginas;

}__attribute__((packed)) t_prot_cpu_mem ;
