/*
 * print-pid-and-tid.c
 *
 * Forkea un proceso y lanza dos hilos por cada uno, imprime PID, Parent PId, y TID de cada uno.
 *
 * Compilar con: gcc -o print-pid-and-tid main.c -lpthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void* printData();
void* printDataAndWait();

int main(int argc, char** argv){

	int pid = fork();
	pthread_t t1, t2;

	if (pid == 0){ // HIJO
		printData();
		pthread_create(&t1, NULL, printDataAndWait, NULL);

		pthread_join(t1, NULL);

	}else if (pid > 0){ // PADRE
		printData();
		pthread_create(&t1, NULL, printDataAndWait, NULL);

		pthread_join(t1, NULL);


	} else { //Error
		perror("forkeando");
	}

	return EXIT_SUCCESS;
}

void* printDataAndWait(){

	printData();



	return EXIT_SUCCESS;
}

void* printData(){

	FILE * archivo;
	char caracteres[100];
	//caracteres=malloc(sizeof(caracteres));
	printf("llegue");
	archivo=fopen("mcod", "r");
	if(archivo==NULL){
		printf("error");
	}
	else{
		while((feof(archivo))==0)
		{
			char*cadena = fgets(caracteres, 100, archivo);
			printf("%s",cadena);
		}
		fclose(archivo);
		free(caracteres);
		}


	return EXIT_SUCCESS;
}
