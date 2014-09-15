#include <semaphore.h>  /* Semaphore */

#include "broadcast.h"

#define TRUE 1
#define SIZE 5

/* START GLOBALS*/
int buffer[SIZE];
int nextSend = 0;
int waitSend = 0;
int totalReaders[SIZE];

int n_senders;
int n_receivers;

sem_t lock;
sem_t canSend;
sem_t bufferFull;
sem_t mutualExclusion;
sem_t bufferEmpty[SIZE];
/* END GLOBALS */

int inicia(int transmissores, int receptores) {
    int i;
    int rc = 0;

    n_senders = transmissores;
    n_receivers = receptores;

    rc = sem_init(&lock, 0, 1);
    checkResults("sem_init()\n", rc);

    rc = sem_init(&canSend, 0, 1);
    checkResults("sem_init()\n", rc);

    rc = sem_init(&mutualExclusion, 0, 1);
    checkResults("sem_init()\n", rc);

    rc = sem_init(&bufferFull, 0, 0);
    checkResults("sem_init()\n", rc);

    for(i = 0; i < SIZE; i++) {
        rc = sem_init(&bufferEmpty[i], 0, 1);
        checkResults("sem_init()\n", rc);
    }

    for(i = 0; i < SIZE; i++) {
        totalReaders[i] = 0;
    }
}

void envia(int val) {
    int i;

    sem_wait(&canSend);
    sem_wait(&bufferEmpty[nextSend % SIZE]);

    printf(">Inserting %d to position %d(%d)\n", val, nextSend % SIZE, nextSend);
    buffer[nextSend % SIZE] = val;
    nextSend++;
    
    sem_post(&canSend); 

    sem_wait(&lock);
    for(i = 0; i < waitSend; i++)
        sem_post(&bufferFull);
    waitSend = 0;
    sem_post(&lock);
}

int recebe(int position, int thread_id) {
    int item;

    //while(position >= nextSend);
    
    sem_wait(&lock);
    if(position >= nextSend) {
        waitSend++;
        sem_post(&lock);
        sem_wait(&bufferFull);
    } else {
        sem_post(&lock);
    }

    sem_wait(&mutualExclusion);
    totalReaders[position % SIZE]++;
    sem_post(&mutualExclusion);

    item = buffer[position % SIZE];
    printf("<(Thread %d)Reading %d from position %d - %d readers\n", thread_id, item, position % SIZE, totalReaders[position % SIZE]);

    sem_wait(&mutualExclusion);
    if(totalReaders[position % SIZE] >= n_receivers) {
        totalReaders[position % SIZE] = 0;
        sem_post(&bufferEmpty[position % SIZE]);
    }
    sem_post(&mutualExclusion);
}