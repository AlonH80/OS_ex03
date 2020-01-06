
#ifndef EX3_Q2_H
#define EX3_Q2_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <fcntl.h>
#include "ex3_q2_given.h"

#define PRODUCERS_CREATED_STRING "main thread created all producer threads\n"
#define CONSUMERS_CREATED_STRING "main thread created all consumer threads\n"
#define CONSUMERS_ENDED "all consumers terminated\n"
#define PRODUCERS_ENDED "all producers terminated\n"
#define EMPTY_SPOT -1

typedef char BOOL;

void program();
void initProgram();
void initializeBuffer();
void semUnlinker();
void semInitializer();
pthread_t* createThreads(int numOfThreads, void* (*f)());
void signalThreads();
void threadsWaiter(pthread_t* threads, int numOfThreads);
void waitThreadsFinish(pthread_t* producers,
                       pthread_t* consumers,
                       pthread_t* prodWait,
                       pthread_t* prodCons);
void endProgram();

pthread_t* createConsumers();
void* waitForConsumers(void* consumers);
void* consumerLoop();
BOOL consumeNumber(int consId, char* whoAmI);

pthread_t* createProducers();
void* waitForProducers(void* producers);
void* producerLoop();
void generatePrimesProd(int threadId);

#endif
