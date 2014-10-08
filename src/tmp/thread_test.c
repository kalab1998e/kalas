/*****************************************************************************

  FILE NAME   : thread_test.c

  PROJECT     : 

  DESCRIPTION : A first thread program is to print out 'Hello World'.

  ----------------------------------------------------------------------------
  RELEASE NOTE : 

   DATE          REV    REMARK
  ============= ====== =======================================================


 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <semaphore.h>
#include <pthread.h>



/* --------------------------------- DEFS ---------------------------------- */

struct thdata {
    char                letter;
    pthread_t           th;
    sem_t               sync;
    sem_t               start;
};

/* ------------------------------------------------------------------------- */



/* ------------------------------- FUNCTIONS ------------------------------- */


/*****************************************************************************

 FUNCTION    : void *thread_function (void *thdata)

 DESCRIPTION : Thread function.

               * Argument
                 void *

               * Return
                 void *

 ATTENTION   :

 *****************************************************************************/
void *thread_function(void *thdata)
{

    struct thdata       *priv = (struct thdata *)thdata;


    /* sync */
    sem_post(&priv->sync);
    sem_wait(&priv->start);

    /* write my letter */
    putchar(priv->letter);

    /* sync */
    sem_post(&priv->sync);

    /* done */
    return (void *) NULL;

}



/* Main 

 *****************************************************************************/
int main (void)
{

    int                 rtn, i;
    struct thdata       *thdata;
    char                hello[128] = "Hello World!\n";


    /* initialize thread data */
    thdata = calloc(sizeof(struct thdata), strlen(hello));
    if (thdata == NULL) {
        perror("calloc()");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < strlen(hello); i++) {
        thdata[i].letter = hello[i];
        sem_init(&thdata[i].sync, 0, 0);
        sem_init(&thdata[i].start, 0, 0);
        rtn = pthread_create(&thdata[i].th, NULL, thread_function, (void *) (&thdata[i]));
        if (rtn != 0) {
            fprintf(stderr, "pthread_create() #%0d failed for %d.", i, rtn);
            exit(EXIT_FAILURE);
        }
    }


    /* synchronization */
    for (i = 0; i < strlen(hello); i++) {
        sem_wait(&thdata[i].sync);
    }

    /* let thread write his letter */
    for (i = 0; i < strlen(hello); i++) {
        sem_post(&thdata[i].start);
        sem_wait(&thdata[i].sync);
    }

    /* join */
    for (i = 0; i < strlen(hello); i++) {
        pthread_join(thdata[i].th, NULL);
        sem_destroy(&thdata[i].sync);
        sem_destroy(&thdata[i].start);
    }


    free(thdata);
    exit(EXIT_SUCCESS);

}
/* ------------------------------------------------------------------------- */
