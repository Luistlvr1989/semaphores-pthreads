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
sem_t waitSender;

// Mutual Exclusion
sem_t mExclusionW;
sem_t mExclusionR;

// Semaphores for Full and Empty Buffer
sem_t bufferFull[SIZE];
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

    rc = sem_init(&waitSender, 0, 0);
    checkResults("sem_init()\n", rc);

    rc = sem_init(&mExclusionW, 0, 1);
    checkResults("sem_init()\n", rc);

    rc = sem_init(&mExclusionR, 0, 1);
    checkResults("sem_init()\n", rc);

    for(i = 0; i < SIZE; i++) {
        rc = sem_init(&bufferFull[i], 0, 0);
        checkResults("sem_init()\n", rc);
    }

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

    /*!<
        < await(bufferEmpty[tmpSend] > 0) bufferEmpty[tmpSend]--; >
            buffer[tmpSend] = val;
        < bufferFull[tmpSend] = n_receivers; >
    */

    for(i = 0; i < n_receivers; i++)
        sem_post(&bufferFull[tmpSend]);

    sem_wait(&lock);
    for(i = 0; i < waitSend; i++)
        sem_post(&waitSender);
    waitSend = 0;
    sem_post(&lock);
}

int recebe(int id) {
    int item, position;

    position = current[id] % SIZE;

    /*!<
        < await(current[id] < nextSend and bufferFull[position] > 0) bufferFull[position]--; >
            item = buffer[position];
            current[id]++;
    */

    sem_wait(&lock);
    if(current[id] >= nextSend) {
        waitSend++;
        sem_post(&lock);
        sem_wait(&waitSender);
    } else {
        sem_post(&lock);
    }

    sem_wait(&bufferFull[position]);

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