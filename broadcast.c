#include "broadcast.h"

/* START GLOBALS*/
int buffer[SIZE];
int totalReaders[SIZE];

// Counters
int nextSend = 0;
int waitSend = 0;

// Receivers Info
int n_receivers;
int* current;

// Lock for waitSend
sem_t lock;

// Mutual Exclusion
sem_t mExclusionW;
sem_t mExclusionR;

// Semaphores for Full and Empty Buffer
sem_t bufferFull;
sem_t bufferEmpty[SIZE];
/* END GLOBALS */

int inicia(int transmissores, int receptores) {
    int i;
    int rc = 0;

    n_receivers = receptores;
    current = malloc(n_receivers * sizeof(int*));

    for(i = 0; i < n_receivers; i++) 
        current[i] = 0;

    for(i = 0; i < SIZE; i++)
        totalReaders[i] = 0;

    rc = sem_init(&lock, 0, 1);
    checkResults("sem_init()\n", rc);

    rc = sem_init(&mExclusionW, 0, 1);
    checkResults("sem_init()\n", rc);

    rc = sem_init(&mExclusionR, 0, 1);
    checkResults("sem_init()\n", rc);

    rc = sem_init(&bufferFull, 0, 0);
    checkResults("sem_init()\n", rc);

    for(i = 0; i < SIZE; i++) {
        rc = sem_init(&bufferEmpty[i], 0, 1);
        checkResults("sem_init()\n", rc);
    }

    return TRUE;
}

void envia(int val) {
    int i, tmpSend;

    sem_wait(&mExclusionW);
    tmpSend = nextSend % SIZE;
    nextSend++;
    sem_post(&mExclusionW); 

    sem_wait(&bufferEmpty[tmpSend]);

    buffer[tmpSend] = val;
    printf(">Inserting %d to position %d\n", val, tmpSend);

    sem_wait(&lock);
    for(i = 0; i < waitSend; i++)
        sem_post(&bufferFull);
    waitSend = 0;
    sem_post(&lock);
}

int recebe(int id) {
    int item, position;

    position = current[id] % SIZE;
    
    sem_wait(&lock);
    if(current[id] >= nextSend) {
        waitSend++;
        sem_post(&lock);
        sem_wait(&bufferFull);
    } else {
        sem_post(&lock);
    }

    item = buffer[position];
    printf("<(Thread %d)Reading %d from position %d\n", id, item, position);
    current[id]++;

    sem_wait(&mExclusionR);
    totalReaders[position]++;
    if(totalReaders[position] >= n_receivers) {
        totalReaders[position] = 0;
        sem_post(&bufferEmpty[position]);
    }
    sem_post(&mExclusionR);

    return item;
}