/*
 * LibSwap.h
 *
 *  Created on: 24/9/2015
 *      Author: utnso
 */

#ifndef LIBSWAP_H_
#define LIBSWAP_H_

void shell(int listener, int skEmisor, int skReceptor, char * buf, int nbytes);

void *get_in_addr(struct sockaddr *sa);

void limpiar (char *cadena);

int tamaniobuf(char cad[]);

int esComando(char * comando);

char* crearArchivoSwap(char *nombre_Swap ,long tamanio);

#endif /* LIBSWAP_H_ */
