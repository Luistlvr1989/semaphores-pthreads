#include <pthread.h>    /* POSIX Threads */
#include <unistd.h>     /* Symbolic Constants */
#include <limits.h>     /* Limits of the Data Types */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <string.h>     /* String handling */

#include "broadcast.h"

/* START GLOBALS*/
int N_SENDERS;
int N_RECEIVERS;

pthread_t* senders;
pthread_t* receivers;
/* END GLOBALS */

/* Prototypes of the functions */
void* senderThread(void* param);
void* receiverThread(void* param);

int main(int argc, char** argv) {
	int i, j;
	int rc = 0;
	char* p;
	//int* id;

	if(argc != 3) {
	 	printf("Specify the senders and receivers\n");
		exit(EXIT_FAILURE);
	}

	errno = 0;
	N_SENDERS   = strtol(argv[1], &p, 10);
	if(errno != 0 || *p != '\0' || N_SENDERS > INT_MAX) {
		printf("The number of senders is not valid\n");
		exit(EXIT_FAILURE);
	}

	errno = 0;
	N_RECEIVERS = strtol(argv[2], &p, 10);
	if(errno != 0 || *p != '\0' || N_RECEIVERS > INT_MAX) {
		printf("The number of receivers is not valid\n");
		exit(EXIT_FAILURE);
	}

	senders   = malloc(N_SENDERS * sizeof(pthread_t*));
	receivers = malloc(N_RECEIVERS * sizeof(pthread_t*));

	inicia(N_SENDERS, N_RECEIVERS);

	for(i = 0; i < N_SENDERS; i++) { 
		int* id = malloc(sizeof(*id));
		*id = i;

        rc = pthread_create(&senders[i], NULL, senderThread, (void*) id);
        checkResults("pthread_create()\n", rc);
    }
    
    for(i = 0; i < N_RECEIVERS; i++) { 
		int* id = malloc(sizeof(*id));
		*id = i;

        rc = pthread_create(&receivers[i], NULL, receiverThread, (void*) id);
        checkResults("pthread_create()\n", rc);
    }
    
    for(i = 0; i < N_SENDERS; i++) { 
        rc = pthread_join(senders[i], NULL);
        checkResults("pthread_join()\n", rc);
    }

    for(i = 0; i < N_RECEIVERS; i++) { 
        rc = pthread_join(receivers[i], NULL);
        checkResults("pthread_join()\n", rc);
    }

    free(senders);
    free(receivers);

    return 0;
}

/**
 * [senderThread description]
 * @param param [description]
 */
void* senderThread(void* param) {
   	int value;
   	value = *((int*) param);

   	envia(value);

    return NULL;
}

/**
 * [receiverThread description]
 * @param param [description]
 */
void* receiverThread(void* param) {
	int value;
   	value = *((int*) param);

	int i;
	
	for(i = 0; i < N_SENDERS; i++)
		recebe(i, value);

    return NULL;
}

