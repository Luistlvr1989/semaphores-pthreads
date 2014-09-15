#ifndef BROADCAST_H
#define BROADCAST_H

#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */

#define checkResults(string, val) {             	 \
	if(val) {                                     	 \
   		printf("Failed with %d at %s", val, string); \
   		exit(EXIT_FAILURE);                                     \
 	}                                              	 \
}

/**
 * [inicia description]
 * @param  transmissores [description]
 * @param  receptores    [description]
 * @return               [description]
 */
int inicia(int transmissores, int receptores);

/**
 * [envia description]
 * @param val [description]
 */
void envia(int val);

/**
 * [recebe description]
 * @param  id [description]
 * @return    [description]
 */
int recebe(int position, int thread_id);

#endif