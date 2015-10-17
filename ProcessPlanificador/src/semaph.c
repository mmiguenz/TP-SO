#include "semaph.h"

/****************************************************************************
* Crea un semaforo con un valor inicial especificado.
* Si el semaforo existe, no se inicializa (por su puesto).
* Se devuelve la identidad del semaforo si todo va bien, si no -1.
*/
int semCreate(key_t key, int initval) {
   register int      id, semval;
   union semun {
      int               val;
      struct semid_ds   *buf;
      ushort            *array;
   } semctl_arg;

   if (key == IPC_PRIVATE)
      return(-1);   /* no utilizable para semaforos privados */

   else if (key == (key_t) -1)
      return(-1);   /* probablemente una llamada erronea anterior a ftok() */

deNuevo:
   if ( (id = semget(key, 3, 0666 | IPC_CREAT)) < 0)
      return(-1);   /* problemas de permisos o tablas llenas */

   /* Cuando se crea el semaforo, sabemos que el valor de todos los
   * miembros es 0.
   *
   * Bloquear el semaforo esperando a que [2] sea 0, e incrementarlo.
   *
   * Hay una condicion de carrera: Cabe la posibilidad de que entre el
   * semget() de arriba y el semop() de abajo, otro proceso pueda llamar
   * a semClose() que puede borrar el semaforo si el ultimo lo esta
   * usando.
   *
   * Ademas, se maneja la condicion de error sobre el identificador.
   * Si esto ocurre, se vuelve atras y se intenta crear de nuevo.
   */

   if (semop(id, &op_lock[0], 2) < 0) {
      if (errno == EINVAL)
         goto deNuevo;
      fprintf(stderr, "semCreate: no puedo bloquear\n");
   }

   /* Obtener el valor del contador de procesos. Si es igual a 0,
   * entonces ninguno ha inicializado el semaforo aun.
   */
   if ( (semval = semctl(id, 1, GETVAL, 0)) < 0)
      fprintf(stderr, "semCreate: no puedo realizar GETVAL\n");

   if (semval == 0) {
      /* Podriamos inicializar mediante SETALL, pero podria borrar el
      * ajuste del valor que se realizo cuando se bloqueo el semaforo antes.
      * En su lugar, se hacen dos llamadas al sistema para inicializar
      * [0] y [1].
      */

      semctl_arg.val = initval;
      if (semctl(id, 0, SETVAL, semctl_arg) < 0)
         fprintf(stderr, "semCreate: puedo SETVAL[0]\n");

      semctl_arg.val = BIGCOUNT;
      if (semctl(id, 1, SETVAL, semctl_arg) < 0)
         fprintf(stderr, "semCreate: puedo SETVAL[1]\n");
   }

   /* Decrementar el contador de procesos y desbloquear.
   */

   if (semop(id, &op_endcreate[0], 2) < 0)
      fprintf(stderr, "semCreate: no puedo acabar semCreate()\n");

   return(id);
}

/****************************************************************************
* Abre un semaforo que debe existir ya.
* Esta funcion deberia de usarse, en vez de semCreate(), si en la llamada
* se sabe que el semaforo deberia ya existir. Por ejemplo un cliente
* de un par cliente-servidor podria utilizarla, si es responsabilidad del
* servidor crear el semaforo.
* Se vuelve la identidad del semaforo si va bien, si no -1.
*/
int semOpen(key_t key) {
   register int   id;

   if (key == IPC_PRIVATE)
      return(-1);   /* no utilizable para semaforos privados */

   else if (key == (key_t) -1)
      return(-1);   /* probablemente una llamada erronea anterior a ftok() */

   if ( (id = semget(key, 3, 0)) < 0)
      return(-1);   /* no existe o las tablas estan llenas */

   /* Decrementa el contador de procesos. No necesitamos un bloqueo
   *  para hacer esto.
   */
   if (semop(id, &op_open[0], 1) < 0)
      fprintf(stderr, "semOpen: no puedo abrir\n");

   return(id);
}

/****************************************************************************
* Borrar un semaforo.
* Se supone que esta llamada se realiza desde un servidor en operaciones como
* apagarServidor ... No importa si los otros procesos estan usandolo o no.
* El resto de los procesos deberian emplear semClose().
*/
void semRm(int id)
{
   if (semctl(id, 0, IPC_RMID, 0) < 0)
      fprintf(stderr, "semRm: no puedo borrar semaforo (IPC_RMID)\n");
}

/****************************************************************************
* Cerrar el semaforo.
* Funcion por proceso que decrementa el numero de procesos activos en el
* semaforo. Se emplea al salir. Si el proceso es el ultimo destruye el
* semaforo.
*/

void semClose(int id) {
   register int   semval;

   /* En primer lugar bloquear el recurso semaforo e incrementar el contador
   * de procesos [1].
   */
   if (semop(id, &op_close[0], 3) < 0)
      fprintf(stderr, "semClose: no puedo bloquer en semClosep\n");

   /* Comprobar si el valor leido es la ultima referencia al semaforo.
    */
   if ( (semval = semctl(id, 1, GETVAL, 0)) < 0)
      fprintf(stderr, "semClose: no puedo realizar GETVAL\n");

   if (semval > BIGCOUNT)
      fprintf(stderr, "< BIGCOUNT>>\n");
   else if (semval == BIGCOUNT)
      semRm(id);
   else if (semop(id, &op_unlock[0], 1) < 0)
         fprintf(stderr, "semClose: no puedo desbloquear\n"); /* desbloqueo */
}

/****************************************************************************
* Espera hasta que el valor del semaforo sea mayor que 0, entonces
* decrementa en 1 y vuelve. Operador wait, DOWN (Tanenbaum) o P (Dijkstra).
*/
void semWait(int id) {
   semOp(id, -1);
}

/****************************************************************************
* Incrementar el semaforo en 1. Operador segnal, UP (Tanenbaum) o
* V (Dijkstra).
*/
void semSignal(int id) {
   semOp(id, 1);
}

/****************************************************************************
* Operacion generica de semaforo:
* incrementar o decrementar cierta cantidad positiva o negativa, distinta
* de cero.
*/
void semOp(int id, int value) {
   if ( (op_op[0].sem_op = value) == 0)
      (void) fprintf(stderr, "semOp: 'valor' no puede ser 0\n");

   if (semop(id, &op_op[0], 1) < 0)
      (void) fprintf(stderr, "semOp: error\n");
}


