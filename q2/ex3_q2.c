
#include "ex3_q2.h"

lluint buffer[BUF_SIZE];
lluint inBuffer = 0;
int prodsIn = TOTAL_MSG;
int prodsOut = TOTAL_MSG;
sem_t* semInBuff;
sem_t* semOutBuff;
pthread_mutex_t ioLock;
pthread_mutex_t buffLock;
pthread_mutex_t getBuffLock;
pthread_mutex_t putBuffLock;
pthread_mutex_t assignIdProd;
pthread_mutex_t assignIdCons;
int numProducers = 0;
int numConsumers = 0;

void initProgram(){
    pthread_mutex_init(&ioLock, NULL);
    pthread_mutex_init(&buffLock, NULL);
    pthread_mutex_init(&getBuffLock, NULL);
    pthread_mutex_init(&putBuffLock, NULL);
    pthread_mutex_init(&assignIdProd, NULL);
    pthread_mutex_init(&assignIdCons, NULL);
    if (sem_unlink("/inBuff") == 0){
        fprintf(stderr, "successul unlink of /inBuff\n");
    }
    if (sem_unlink("/outBuff") == 0){
        fprintf(stderr, "successul unlink of /outBuff\n");
    }

    semInBuff = sem_open("/inBuff", O_CREAT, S_IRWXU, 0);
    if (semInBuff == SEM_FAILED)
    {
        perror("failed to open semaphore /inBuff\n");
        exit(EXIT_FAILURE);
    }

    semOutBuff = sem_open("outBuff", O_CREAT, S_IRWXU, 0);
    if (semOutBuff == SEM_FAILED)
    {
        perror("failed to open semaphore /outBuff\n");
        exit(EXIT_FAILURE);
    }
}

void endProgram(){
    pthread_mutex_destroy(&ioLock);
    pthread_mutex_destroy(&buffLock);
    pthread_mutex_destroy(&getBuffLock);
    pthread_mutex_destroy(&putBuffLock);
    pthread_mutex_destroy(&assignIdProd);
    pthread_mutex_destroy(&assignIdCons);
    if (sem_unlink("/inBuff") == 0){
        fprintf(stderr, "successul unlink of /inBuff\n");
    }
    if (sem_unlink("/outBuff") == 0){
        fprintf(stderr, "successul unlink of /outBuff\n");
    }

    sem_close(semInBuff);
    sem_close(semOutBuff);
}

pthread_t* createConsumers(){
    return createThreads(N_CONS, consumerLoop);
}

pthread_t* createProducers(){
    return createThreads(N_PROD, producerLoop);
}

pthread_t* createThreads(int numOfThreads, void* (*f)()){
    int i;
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * numOfThreads);
    for (i = 0; i < numOfThreads; ++i){
        pthread_create(threads + i, NULL, f, NULL);
    }

    return threads;
}

void* waitForProducers(void* producers){
    threadsWaiter(producers, N_PROD);
    pthread_mutex_lock(&ioLock);
    printf(PRODUCERS_ENDED);
    pthread_mutex_unlock(&ioLock);
    return NULL;
}

void* waitForConsumers(void* consumers){
    threadsWaiter(consumers, N_CONS);
    pthread_mutex_lock(&ioLock);
    printf(CONSUMERS_ENDED);
    pthread_mutex_unlock(&ioLock);
    return NULL;
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

    pthread_create(&prodWait, NULL, waitForProducers, producers);
    pthread_create(&consWait, NULL, waitForConsumers, consumers);
    pthread_join(prodWait, NULL);
    pthread_join(consWait, NULL);

    endProgram();
}

void generatePrimesProd(int threadId){
    lluint n1 = get_random_in_range();
    while(!is_prime(n1) && prodsIn > 0){
        n1 = get_random_in_range();
    }

    lluint n2 = get_random_in_range();
    while(!is_prime(n2) && prodsIn > 0){
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

void add_to_buf(lluint prod){

    if (inBuffer == BUF_SIZE){
        sem_wait(semOutBuff);
    }

    pthread_mutex_lock(&buffLock);
    buffer[inBuffer++] = prod;
    if (inBuffer == 1){
        sem_post(semInBuff);
    }
    pthread_mutex_unlock(&buffLock);

    --prodsIn;
}

void remove_from_buf(lluint* prod){

    if(inBuffer == 0){
        sem_wait(semInBuff);
    }
    pthread_mutex_lock(&buffLock);
    pthread_mutex_lock(&ioLock);
    printf("consumer inBuffer = %d\n", inBuffer);
    pthread_mutex_unlock(&ioLock);
    (*prod) = buffer[--inBuffer];
    if (inBuffer == BUF_SIZE - 1){
        sem_post(semOutBuff);
    }

    pthread_mutex_unlock(&buffLock);

    --prodsOut;

}

void* consumerLoop(){
    pthread_mutex_lock(&assignIdCons);
    int threadId = ++numConsumers;
    pthread_mutex_unlock(&assignIdCons);

    lluint currProd = 0, n1, n2, dontBother = 0;


    char* whoAmI = (char*)malloc(20);
    sprintf(whoAmI, "consumer #%d", threadId);
    while(prodsOut > 0){
        pthread_mutex_lock(&getBuffLock);
        if(prodsOut > 0) {
            remove_from_buf(&currProd);

            pthread_mutex_lock(&ioLock);
            write_remove_from_buf_msg(threadId, &currProd);
            pthread_mutex_unlock(&ioLock);
        }
        else{
            dontBother = 1;
        }
        pthread_mutex_unlock(&getBuffLock);

        if(!dontBother) {
            find_two_factors(currProd, &n1, &n2);

            pthread_mutex_lock(&ioLock);
            write_product(whoAmI, n1, n2, currProd);
            pthread_mutex_unlock(&ioLock);
        }
    }

    free(whoAmI);
    pthread_mutex_lock(&ioLock);
    write_consumer_is_done(threadId);
    pthread_mutex_unlock(&ioLock);

    return NULL;
}

void* producerLoop(){
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

int main(int argc, char* argv[]){
    programLoop();
}