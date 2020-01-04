//TODO: Remove "inBufer" debug line
//TODO: Insert comments


#ifndef EX3_Q2_H
#define EX3_Q2_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "ex3_q2_given.h"

#define PRODUCERS_CREATED_STRING "main thread created all producer threads\n"
#define CONSUMERS_CREATED_STRING "main thread created all consumer threads\n"
#define CONSUMERS_ENDED "all consumers terminated\n"
#define PRODUCERS_ENDED "all producers terminated\n"
#define EMPTY_SPOT -1

void programLoop();
pthread_t* createThreads(int numOfThreads, void* (*f)());
void threadsWaiter(pthread_t* threads, int numOfThreads);
void initProgram();
void endProgram();
void initializeBuffer();
void semUnlinker();
void semInitializer();

void* consumerLoop();
pthread_t* createConsumers();
void* waitForConsumers(void* consumers);

void* producerLoop();
void generatePrimesProd(int threadId);
pthread_t* createProducers();
void* waitForProducers(void* producers);

#endif
