/*
 * semaph.h
 *
 *  Created on: 16/10/2015
 *      Author: utnso
 */

#ifndef SEMAPH_H_
#define SEMAPH_H_




/*
* Provee un interface mas sencillo de entender que las llamadas a sistema
* de semaforos de System V. Hay 7 rutinas disponibles:
*
*   id = semCreate(key, initval);   # Crear con un valor inicial o abrir.
*   id = semOpen(key);               # Abrir (debe existir ya)
*        semWait(id);                 # espera = P = down en 1
*        semSignal(id);               # senal  = V = up   en 1
*        semOp(id, cantidad);        # espera   si (cantidad < 0)
*                                            # senal    si (cantidad > 0)
*        semClose(id);                  # cierra
*        semRm(id);                     # destruye (borra)
*
* Se disegna un semaforo soportado por un conjunto de tres, dos de ellos
* auxiliares. (Los semaforos se crean por arrays)
*   - El primer miembro, [0], es el valor real del semaforo.
*   - El segundo miembro, [1], es un contador utilizado para conocer
*     si todos los procesos han acabado con el semaforo. El contador
*     se inicializa con un numero grande (BIGCOUNT) y se decrementa cada
*     vez que se crea o abre, y se incrementa en cada cierre.
*
*     De esta forma se puede "ajustar" la caracteristica de System V
*     de forma que se tenga en cuenta cualquier proceso que salga
*     sin llamar a semClose(). A pesar de ello, no ahuda mucho si el
*     ultimo proceso sale sin cerrar el semaforo, ya que no hay forma
*     de destruir el semaforo, pero puede ayudar si acaba (intencional
*     o no intencionalmente) cualquier otro proceso diferente del ultimo.
*   - El tercer miembro, [2], del conjunto de semaforos se utiliza para
*     bloquear las secciones criticas en semCreate() y semClose().
*/

#include   <stdio.h>
#include   <sys/types.h>
#include   <sys/ipc.h>
#include   <sys/sem.h>
#include   <errno.h>

void   semOp(int, int);
int    semCreate(key_t, int);
int    semOpen(key_t);
void   semRm(int);
void   semClose(int);
void   semWait(int);
void   semSignal(int);

#define   BIGCOUNT   10000 /* Valor inicial para el contador de procesos */

/* Define los arrays de operaciones del semaforo para llamadas a
* semop().
*/
static struct sembuf   op_lock[2] = {
   2, 0, 0,         /* espera para [2] (bloqueo) sea igual 0
                     * despues incrementa [2] en 1 - esto lo bloquea */
   2, 1, SEM_UNDO   /* UNDO para liberar el bloqueo si el proceso sale
		     * antes de desbloquear explicitamente */
};

static struct sembuf   op_endcreate[2] = {
   1, -1, SEM_UNDO, /* decrementa [1] (contador de procesos) con undo en
		     * caso de finalizar */
                    /* UNDO para ajustar el contador de procesos en caso de
		     * acabar antes de llamar explicitamente a semClose() */
   2, -1, SEM_UNDO  /* despues decrementa [2] (bloqueo) de vuelta a 0 */
};

static struct sembuf   op_open[1] = {
   1, -1, SEM_UNDO   /* decrementa [1] (contador de proceso) con undo en
		      * caso de finalizar */
};

static struct sembuf   op_close[3] = {
   2, 0, 0,          /* espera hasta que [2] (bloqueo) sea igual a 0 */
   2, 1, SEM_UNDO,   /* despues incrementa [2] en 1 - esto lo bloquea */
   1, 1, SEM_UNDO    /* despues incrementa [1] (contador de procesos) */
};

static struct sembuf   op_unlock[1] = {
   2, -1, SEM_UNDO   /* decrementa [2] (bloqueo) de vuelta a 0 */
};

static struct sembuf   op_op[1] = {
   0, 99, SEM_UNDO   /* decrementa o incrementa [0] con undo en caso de
		      * finalizar */
                     /* El 99 se substituye con la cantidad real que hay
		      * que substraer (positiva o negativa) */
};




#endif /* SEMAPH_H_ */
