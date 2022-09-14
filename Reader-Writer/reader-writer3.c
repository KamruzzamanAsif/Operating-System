#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

int cnt = 1; // the resource
int readcount = 0, writecount = 0;
sem_t rmutex, serviceQueue, resource;


void *writer(void *wno)
{   
    // enter
    sem_wait(&serviceQueue);

    sem_wait(&resource);
    // Writing section (critical section)
    cnt = cnt*2;
    printf("Writer %d modified cnt to %d\n",(*((int *)wno)),cnt);
    sem_post(&resource);


    // exit
    sem_post(&serviceQueue);
}

void *reader(void *rno)
{   
    // enter
    sem_wait(&serviceQueue);

    sem_wait(&rmutex);
    readcount += 1;
    if(readcount == 1)
        sem_wait(&resource);
    sem_post(&rmutex);

    sem_post(&serviceQueue);

    // Reading Section (critical section)
    printf("Reader %d: read cnt as %d\n",*((int *)rno),cnt);

    // exit
    sem_wait(&rmutex);
    readcount -= 1; 
    if(readcount == 0)
        sem_post(&resource);
    sem_post(&rmutex);
}

int main()
{   
    pthread_t read[10],write[10]; // thread declarations
    
    sem_init(&rmutex, 0, 1); 
    sem_init(&serviceQueue, 0, 1);
    sem_init(&resource, 0, 1);


    int a[10] = {1,2,3,4,5,6,7,8,9,10}; //Just used for numbering the reader and writer

    for(int i = 0; i < 10; i++) {
        pthread_create(&read[i], NULL, (void *)reader, (void *)&a[i]); //a[i] means passing the reader thread number
        pthread_create(&write[i], NULL, (void *)writer, (void *)&a[i]); //a[i] means passing the writer thread number
    }


    // thread join
    for(int i = 0; i < 10; i++) {
        pthread_join(read[i], NULL);
    }
    for(int i = 0; i < 5; i++) {
        pthread_join(write[i], NULL);
    }


    sem_destroy(&rmutex);
    sem_destroy(&serviceQueue);
    sem_destroy(&resource);

    return 0;
}
