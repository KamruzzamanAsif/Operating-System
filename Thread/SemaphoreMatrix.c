#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>

// size of matrix
#define COL 8000
#define ROW 7000

int matA[ROW][COL];

// number of threads
#define MAX_THREAD 7000

sem_t mutex;
int  count = 0;

void* counter(void* arg)
{
    int i = *(int*) arg; 
   sem_wait(&mutex);

   for(int j=0; j<COL; j++){
        if(matA[i][j]==1) count++;
   }

    sem_post(&mutex);
}


int main()
{

    int i, j;
    for (i = 0; i < ROW; ++i) {
        for (j = 0; j < COL; ++j) {
          matA[ i ][ j ] = 1;
        }
    }

    // semaphore initialize
    sem_init(&mutex, 0, 1);

	// declaring threads
	pthread_t threads[MAX_THREAD];

	// create threads
    
	for (int i = 0; i < MAX_THREAD; i++) {
        int* tid; // for row/step number of the resultant martrix
        tid = (int *) malloc( sizeof(int) );
        *tid = i;

        pthread_create(&threads[i], NULL, counter, (void*)(tid));
    }


    // join
    for (int i=0; i<MAX_THREAD; i++)
        pthread_join(threads[i], NULL);
    
    sem_destroy(&mutex);
    
    printf("total 1 is: %d\n", count);

	return 0;
}
