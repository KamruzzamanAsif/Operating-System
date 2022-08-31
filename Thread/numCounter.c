#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

// size of matrix
#define COL 80
#define ROW 3

int matA[ROW][COL];

// number of threads
#define MAX_THREAD 3

int count = 0;

void* counter(void* arg)
{
   int i = *(int*) arg; 

   for(int j=0; j<COL; j++){
        if(matA[i][j]==1) count++;
   }
}


int main()
{

    int i, j;
    for (i = 0; i < ROW; ++i) {
        for (j = 0; j < COL; ++j) {
          matA[ i ][ j ] = 1;
        }
    }


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
    
    printf("total 1 is: %d\n", count);

	return 0;
}
