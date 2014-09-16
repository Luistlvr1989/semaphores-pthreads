#ifndef BROADCAST_H
#define BROADCAST_H

#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <semaphore.h>  /* Semaphore */

#define TRUE 1
#define SIZE 5

#define checkResults(string, val) {             	 \
	if(val) {                                     	 \
   		printf("Failed with %d at %s", val, string); \
   		exit(EXIT_FAILURE);                                     \
 	}                                              	 \
}

/**
 * [inicia Initialize all the variables]
 * @param  transmissores [Number of senders]
 * @param  receptores    [Number of receivers]
 * @return               [TRUE if success]
 */
int inicia(int transmissores, int receptores);

/**
 * [envia Sends a value using a limited buffer]
 * @param val [The value to be send]
 */
void envia(int val);

/**
 * [recebe Receipts a value sent by the function envia()]
 * @param  id [The id of the receiver thread]
 * @return    [the value]
 */
int recebe(int id);

#endif