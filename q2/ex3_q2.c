
#include "ex3_q2.h"
#include "globals.h"

void program(){
    pthread_t *consumers, *producers;
    pthread_t consWait, prodWait;
    // workflow
    initProgram();
    producers = createProducers();
    consumers = createConsumers();
    signalThreads();
    waitThreadsFinish(producers, consumers, &prodWait, &consWait);
    endProgram();
}

void initProgram(){
    // Initialize global variables, mutexes and semaphores
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

    semUnlinker();
    semInitializer();
}

void initializeBuffer(){
    // set EMPTY_SPOT in all the buffer's cells
    int i;
    for (i = 0; i < BUF_SIZE; ++i){
        buffer[i] = EMPTY_SPOT;
    }
}

void semUnlinker(){
    // Unlink semaphores used in the program
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
    // Initialize global semaphores used in the prgoram
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

pthread_t* createThreads(int numOfThreads, void* (*f)()){
    // This function creates numOfThreads threads running f
    // Return value: array of the created threads ids
    int i;
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * numOfThreads);
    for (i = 0; i < numOfThreads; ++i){
        pthread_create(&threads[i], NULL, f, NULL);
    }

    return threads;
}

void signalThreads(){
    // Signal threads they can start work
    int i;
    for (i = 0; i < N_PROD + N_CONS; ++i){
        sem_post(semNumOfThreads);
    }
}

void threadsWaiter(pthread_t* threads, int numOfThreads){
    // This function wait for all the threads in threads array to finish
    int i;
    for (i = 0; i < numOfThreads; ++i){
        pthread_join(threads[i], NULL);
    }
}

void waitThreadsFinish(pthread_t* producers, pthread_t* consumers, pthread_t* prodWait, pthread_t* consWait){
    // This function create 2 additional threads,
    // that waits for the producers and consumers thread to be done.
    pthread_create(prodWait, NULL, waitForProducers, producers);
    pthread_create(consWait, NULL, waitForConsumers, consumers);
    pthread_join(*prodWait, NULL);
    pthread_join(*consWait, NULL);
}

void endProgram(){
    // This function contains all the actions to be done in the end of the program.
    // destroy mutexes, unlink and close semaphores.
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

void add_to_buf(lluint prod){
    buffer[nextInBuffer++] = prod;
    nextInBuffer %= BUF_SIZE;
}

void remove_from_buf(lluint* prod){
    (*prod) = buffer[nextOutBuffer];
    buffer[nextOutBuffer] = EMPTY_SPOT;
    nextOutBuffer = (nextOutBuffer + 1) % BUF_SIZE;
}

pthread_t* createConsumers(){
    // This function create N_CONS threads, with function consumerLoop,
    // and prints message in the end of the loop.
    // Return value - array of pthread_t contains the thread id of the created threads.
    pthread_t* consumersThreads = createThreads(N_CONS, consumerLoop);
    pthread_mutex_lock(&ioLock);
    printf(CONSUMERS_CREATED_STRING);
    pthread_mutex_unlock(&ioLock);
    return consumersThreads;
}

void* waitForConsumers(void* consumers){
    // Wait for consumers threads to finish.
    threadsWaiter(consumers, N_CONS);
    pthread_mutex_lock(&ioLock);
    printf(CONSUMERS_ENDED);
    pthread_mutex_unlock(&ioLock);
    return NULL;
}

void* consumerLoop(){
    // This function consume TOTAL_MSG numbers from buffer
    int consId;
    char* whoAmI;
    BOOL isDone = 0;

    sem_wait(semNumOfThreads);
    pthread_mutex_lock(&assignIdCons);
    consId = ++numConsumers;
    pthread_mutex_unlock(&assignIdCons);
    whoAmI = (char*)malloc(20);
    sprintf(whoAmI, "consumer #%d", consId);
    while(!isDone){
        isDone = consumeNumber(consId, whoAmI);
    }

    free(whoAmI);
    pthread_mutex_lock(&ioLock);
    write_consumer_is_done(consId);
    pthread_mutex_unlock(&ioLock);
    return NULL;
}

BOOL consumeNumber(int consId, char* whoAmI){
    // Consume number from buffer, and find the prime factors
    lluint currProd = 0, n1, n2;
    BOOL isDone = 0;

    pthread_mutex_lock(&getBuffLock);
    if(prodsOut > 0) {
        pthread_mutex_lock(&buffLock);
        while (buffer[nextOutBuffer] == EMPTY_SPOT){
            waitBufferEmpty = 1;
            pthread_cond_wait(&waitBuff, &buffLock);
            // cond_wait may accidentally release wait
        }
        remove_from_buf(&currProd);
        if (waitBufferFull == 1){
            waitBufferFull = 0;
            pthread_cond_signal(&waitBuff);
        }
        pthread_mutex_unlock(&buffLock);
        --prodsOut;

        pthread_mutex_lock(&ioLock);
        write_remove_from_buf_msg(consId, &currProd);
        pthread_mutex_unlock(&ioLock);

        pthread_mutex_unlock(&getBuffLock);

        find_two_factors(currProd, &n1, &n2);
        pthread_mutex_lock(&ioLock);
        write_product(whoAmI, n1, n2, currProd);
        pthread_mutex_unlock(&ioLock);
    }
    else{
        pthread_mutex_unlock(&getBuffLock);
        isDone = 1;
    }
    return isDone;
}

pthread_t* createProducers(){
    // This function create N_PROD threads, with function producerLoop,
    // and prints message in the end of the loop.
    // Return value - array of pthread_t contains the thread id of the created threads.
    pthread_t* producersThreads = createThreads(N_PROD, producerLoop);
    pthread_mutex_lock(&ioLock);
    printf(PRODUCERS_CREATED_STRING);
    pthread_mutex_unlock(&ioLock);
    return producersThreads;
}

void* waitForProducers(void* producers){
    // Wait for producers threads to finish.
    threadsWaiter(producers, N_PROD);
    pthread_mutex_lock(&ioLock);
    printf(PRODUCERS_ENDED);
    pthread_mutex_unlock(&ioLock);
    return NULL;
}

void* producerLoop(){
    // This function produce TOTAL_MSG numbers to buffer
    sem_wait(semNumOfThreads);
    pthread_mutex_lock(&assignIdProd);
    int prodId = ++numProducers;
    pthread_mutex_unlock(&assignIdProd);
    srand(((lluint)time(0) % (int)pthread_self()) * prodId);

    while (prodsIn > 0){
        generatePrimesProd(prodId);
    }
    pthread_mutex_lock(&ioLock);
    write_producer_is_done(prodId);
    pthread_mutex_unlock(&ioLock);
    return NULL;
}

void generatePrimesProd(int prodId){
    // this function finds 2 prime factors (n1, n2) in range RAND_LO,RAND_HI,
    // and add the product n1*n2 to the buffer.
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
        pthread_mutex_lock(&buffLock);
        while (buffer[nextInBuffer] != EMPTY_SPOT){
            waitBufferFull = 1;
            pthread_cond_wait(&waitBuff, &buffLock);
            // cond_wait may accidentally release wait
        }
        pthread_mutex_lock(&ioLock);
        write_add_to_buf_msg(prodId, n1, n2, prod);
        pthread_mutex_unlock(&ioLock);

        add_to_buf(prod);
        if (waitBufferEmpty == 1){
            waitBufferEmpty = 0;
            pthread_cond_signal(&waitBuff);
        }
        pthread_mutex_unlock(&buffLock);
        --prodsIn;
    }
    pthread_mutex_unlock(&putBuffLock);
}

int main(int argc, char* argv[]){
    program();
}