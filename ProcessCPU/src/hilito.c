/*
 ============================================================================
 Name        : hilito.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <commons/string.h>
#include <assert.h>

void procesarCadena(char* cadena){
  char* line = cadena;
  char** substrings = string_split(line, " ");

	if (strcmp( substrings[0] ,"iniciar")==0){
		//aviso a memoria substrings[1] = nroPaginas
		// if (hay lugar para ejecutar mcod)
		// return mProc x -iniciado/fallo
		//y a planificador
		printf("\n\n encontro iniciar\n\n");
	}

	if (strcmp(substrings[0] ,"leer")==0){
		//aviso a memoria que lea substrings[1]
		printf("\n\n encontro leer\n\n");

	}
	if (strcmp(substrings[0] ,"finalizar")==0){
		//aviso a memoria y a planificador
		printf("\n\n encontro finalizar\n\n");
	}
						free(substrings[0]);
	                    //free(substrings)
}


void abrirmCod(){

	FILE *archivo;
 	char caracteres[100];
 	//caracteres= malloc(sizeof caracteres); free (caracteres);

 	archivo = fopen("mCod","r");

 				if (archivo == NULL){

 					printf("\nError de apertura del archivo. \n\n");
 			        }else{


 					while (feof(archivo) == 0)
 						{
 							char* cadena = fgets(caracteres,100,archivo);

 							procesarCadena(cadena);


 						}
 			        }
        system("PAUSE");

        fclose(archivo);

}

//int main(void) {

	//pthread_t unHilito;
	//pthread_create(&unHilito, NULL,(void*)abrirmCod, NULL);




//	return EXIT_SUCCESS;
//}
