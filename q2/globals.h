
#ifndef GLOBALS_H
#define GLOBALS_H

lluint buffer[BUF_SIZE];
lluint nextInBuffer = 0;
lluint nextOutBuffer = 0;
int waitBufferFull = 0;
int waitBufferEmpty = 0;
sem_t* semInBuff;
sem_t* semOutBuff;
sem_t* semNumOfThreads;
pthread_mutex_t ioLock;
pthread_mutex_t buffLock;
pthread_mutex_t getBuffLock;
pthread_mutex_t putBuffLock;
pthread_cond_t waitBuff;
int prodsIn;
int prodsOut;
int numConsumers;
pthread_mutex_t assignIdCons;
int numProducers;
pthread_mutex_t assignIdProd;

#endif
