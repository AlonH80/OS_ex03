
#include "ex3_q2.h"
#include "globals.h"


void initProgram(){
    prodsIn = TOTAL_MSG;
    prodsOut = TOTAL_MSG;
    numConsumers = 0;
    numProducers = 0;
    initializeBuffer();
    pthread_mutex_init(&ioLock, NULL);
    pthread_mutex_init(&buffLock, NULL);
    pthread_mutex_init(&getBuffLock, NULL);
    pthread_mutex_init(&putBuffLock, NULL);
    pthread_mutex_init(&assignIdProd, NULL);
    pthread_mutex_init(&assignIdCons, NULL);
    pthread_cond_init(&waitBuff, NULL);
    srand(time(0));

    semUnlinker();
    semInitializer();
}

void endProgram(){
    pthread_mutex_destroy(&ioLock);
    pthread_mutex_destroy(&buffLock);
    pthread_mutex_destroy(&getBuffLock);
    pthread_mutex_destroy(&putBuffLock);
    pthread_mutex_destroy(&assignIdProd);
    pthread_mutex_destroy(&assignIdCons);
    pthread_cond_destroy(&waitBuff);

    semUnlinker();
    sem_close(semInBuff);
    sem_close(semOutBuff);
    sem_close(semNumOfThreads);
}

void semUnlinker(){
    if (sem_unlink("/inBuff") == 0){
        fprintf(stderr, "successul unlink of /inBuff\n");
    }
    if (sem_unlink("/outBuff") == 0){
        fprintf(stderr, "successul unlink of /outBuff\n");
    }
    if (sem_unlink("/numOfThreads") == 0){
        fprintf(stderr, "successul unlink of /numOfThreads\n");
    }
}

void semInitializer(){
    semInBuff = sem_open("/inBuff", O_CREAT, S_IRWXU, 0);
    if (semInBuff == SEM_FAILED)
    {
        perror("failed to open semaphore /inBuff\n");
        exit(EXIT_FAILURE);
    }
    semOutBuff = sem_open("/outBuff", O_CREAT, S_IRWXU, 0);
    if (semOutBuff == SEM_FAILED)
    {
        perror("failed to open semaphore /outBuff\n");
        exit(EXIT_FAILURE);
    }
    semNumOfThreads = sem_open("/numOfThreads", O_CREAT, S_IRWXU, 0);
    if (semOutBuff == SEM_FAILED)
    {
        perror("failed to open semaphore /numOfThreads\n");
        exit(EXIT_FAILURE);
    }
}

void initializeBuffer(){
    int i;
    for (i = 0; i < BUF_SIZE; ++i){
        buffer[i] = EMPTY_SPOT;
    }
}

pthread_t* createThreads(int numOfThreads, void* (*f)()){
    int i;
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * numOfThreads);
    for (i = 0; i < numOfThreads; ++i){
        pthread_create(&threads[i], NULL, f, NULL);
    }

    return threads;
}

void threadsWaiter(pthread_t* threads, int numOfThreads){
    int i;
    for (i = 0; i < numOfThreads; ++i){
        pthread_join(threads[i], NULL);
    }
}

void programLoop(){
    initProgram();

    pthread_t *consumers, *producers;
    pthread_t consWait, prodWait;

    producers = createProducers();
    pthread_mutex_lock(&ioLock);
    printf(PRODUCERS_CREATED_STRING);
    pthread_mutex_unlock(&ioLock);

    consumers = createConsumers();
    pthread_mutex_lock(&ioLock);
    printf(CONSUMERS_CREATED_STRING);
    pthread_mutex_unlock(&ioLock);

    int i;
    for (i = 0; i < N_PROD + N_CONS; ++i){
        sem_post(semNumOfThreads);
    }

    pthread_create(&prodWait, NULL, waitForProducers, producers);
    pthread_create(&consWait, NULL, waitForConsumers, consumers);
    pthread_join(prodWait, NULL);
    pthread_join(consWait, NULL);

    endProgram();
}

void add_to_buf(lluint prod){
    pthread_mutex_lock(&buffLock);
    while (buffer[nextInBuffer] != EMPTY_SPOT){
        waitBufferFull = 1;
        pthread_cond_wait(&waitBuff, &buffLock);
        // cond_wait may accidentally release wait
    }
    buffer[nextInBuffer++] = prod;
    nextInBuffer %= BUF_SIZE;
    if ((nextInBuffer - 1) % BUF_SIZE == nextOutBuffer && waitBufferEmpty == 1){
        waitBufferEmpty = 0;
        pthread_cond_signal(&waitBuff);
    }
    pthread_mutex_unlock(&buffLock);
    --prodsIn;
}

void remove_from_buf(lluint* prod){
    pthread_mutex_lock(&buffLock);
    while (nextInBuffer == nextOutBuffer){
        waitBufferEmpty = 1;
        pthread_cond_wait(&waitBuff, &buffLock);
        // cond_wait may accidentally release wait
    }
    pthread_mutex_lock(&ioLock);
    //printf("consumer inBuffer = %d\n", nextInBuffer);
    pthread_mutex_unlock(&ioLock);
    (*prod) = buffer[nextOutBuffer];
    buffer[nextOutBuffer] = EMPTY_SPOT;
    nextOutBuffer = (nextOutBuffer + 1) % BUF_SIZE;

    if ((nextOutBuffer - 1) % BUF_SIZE == nextInBuffer && waitBufferFull == 1){
        waitBufferFull = 0;
        pthread_cond_signal(&waitBuff);
    }

    pthread_mutex_unlock(&buffLock);

    --prodsOut;

}

void* consumerLoop(){
    sem_wait(semNumOfThreads);
    pthread_mutex_lock(&assignIdCons);
    int threadId = ++numConsumers;
    pthread_mutex_unlock(&assignIdCons);
    lluint currProd = 0, n1, n2;

    char* whoAmI = (char*)malloc(20);
    sprintf(whoAmI, "consumer #%d", threadId);
    while(1){
        pthread_mutex_lock(&getBuffLock);
        if(prodsOut > 0) {
            remove_from_buf(&currProd);

            pthread_mutex_lock(&ioLock);
            write_remove_from_buf_msg(threadId, &currProd);
            pthread_mutex_unlock(&ioLock);
        }
        else{
            pthread_mutex_unlock(&getBuffLock);
            break;
        }
        pthread_mutex_unlock(&getBuffLock);

        find_two_factors(currProd, &n1, &n2);
        pthread_mutex_lock(&ioLock);
        write_product(whoAmI, n1, n2, currProd);
        pthread_mutex_unlock(&ioLock);

    }

    free(whoAmI);
    pthread_mutex_lock(&ioLock);
    write_consumer_is_done(threadId);
    pthread_mutex_unlock(&ioLock);

    return NULL;
}

pthread_t* createConsumers(){
    return createThreads(N_CONS, consumerLoop);
}

void* waitForConsumers(void* consumers){
    threadsWaiter(consumers, N_CONS);
    pthread_mutex_lock(&ioLock);
    printf(CONSUMERS_ENDED);
    pthread_mutex_unlock(&ioLock);
    return NULL;
}

void* producerLoop(){
    sem_wait(semNumOfThreads);
    pthread_mutex_lock(&assignIdProd);
    int threadId = ++numProducers;
    pthread_mutex_unlock(&assignIdProd);

    while (prodsIn > 0){
        generatePrimesProd(threadId);
    }
    pthread_mutex_lock(&ioLock);
    write_producer_is_done(threadId);
    pthread_mutex_unlock(&ioLock);
    return NULL;
}

void generatePrimesProd(int threadId){
    lluint n1 = get_random_in_range();
    while(!is_prime(n1)){
        n1 = get_random_in_range();
    }

    lluint n2 = get_random_in_range();
    while(!is_prime(n2)){
        n2 = get_random_in_range();
    }
    lluint prod = n1 * n2;

    pthread_mutex_lock(&putBuffLock);
    if (prodsIn > 0) {
        pthread_mutex_lock(&ioLock);
        write_add_to_buf_msg(threadId, n1, n2, prod);
        pthread_mutex_unlock(&ioLock);

        add_to_buf(prod);
    }
    pthread_mutex_unlock(&putBuffLock);

}

pthread_t* createProducers(){
    return createThreads(N_PROD, producerLoop);
}

void* waitForProducers(void* producers){
    threadsWaiter(producers, N_PROD);
    pthread_mutex_lock(&ioLock);
    printf(PRODUCERS_ENDED);
    pthread_mutex_unlock(&ioLock);
    return NULL;
}

int main(int argc, char* argv[]){
    programLoop();
}