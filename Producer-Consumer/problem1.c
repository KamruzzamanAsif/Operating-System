/* The producer and consumer share a fixed-size buffer used as a queue. 
The producer’s job is to generate data and put this in the buffer. 
The consumer’s job is to consume the data from this buffer, one at a time. */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define Max_Items  10    // maximum items a producer can produce or a consumer can consume

#define Buffer_Size  10  

#define Max_Producer 10
#define Max_Consumer 10

sem_t empty, full;
pthread_mutex_t mutex;

int input_index = 0; // index where producer will produce
int output_index = 0; // index from where consumer will consume
int buffer[Buffer_Size];


void *do_producer(void *pno){
    int item;

    for(int i = 0; i < Max_Items; i++) {
        item = rand(); // Produce an random item
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        buffer[input_index] = item;
        printf("Producer %d: Insert Item %d at %d\n", *((int *)pno),buffer[input_index],input_index);
        input_index = (input_index+1)%Buffer_Size;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

void *do_consumer(void *cno)
{   
    int item;

    for(int i = 0; i < Max_Items; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        item = buffer[output_index];
        printf("Consumer %d: Remove Item %d from %d\n",*((int *)cno),item, output_index);
        output_index = (output_index+1)%Buffer_Size;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}


int main(void){
    pthread_t producer[Max_Producer], consumer[Max_Consumer];

    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, Buffer_Size);
    sem_init(&full, 0, 0);

    // used for numbering the producer and consumer
    int num[Max_Producer];
    for(int i=1; i<=Max_Producer; i++) num[i] = i+1;

    // producer and consumer thread creation
    for(int i=0; i<Max_Producer; i++)
        pthread_create(&producer[i], NULL, (void *)do_producer, (void *)(&num[i]));
    
    for(int i=0; i<Max_Consumer; i++)
        pthread_create(&consumer[i], NULL, (void *)do_consumer, (void *)(&num[i])); 

    // join the threads
    for(int i = 0; i < Max_Producer; i++) {
        pthread_join(producer[i], NULL);
    }
    for(int i = 0; i < Max_Consumer; i++) {
        pthread_join(consumer[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
    
}