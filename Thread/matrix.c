#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

// size of matrix
#define MAX 9

// number of threads
#define MAX_THREAD 9

int matA[MAX][MAX];
int matB[MAX][MAX];
int matC[MAX][MAX];

void* multi(void* arg)
{
    int i = *(int*) arg; // i is the step/row num
    // printf("i value is :%d \n", i);

	for (int j = 0; j < MAX; j++)
	    for (int k = 0; k < MAX; k++)
		    matC[i][j] += matA[i][k] * matB[k][j];
}


int main()
{
	// Generating random values in matA and matB
	for (int i = 0; i < MAX; i++) {
		for (int j = 0; j < MAX; j++) {
			matA[i][j] = rand() % 10;
			matB[i][j] = rand() % 10;
		}
	}

	// Displaying matA
	printf("\nMatrix A:\n");
	for (int i = 0; i < MAX; i++) {
		for (int j = 0; j < MAX; j++)
			printf("%d ", matA[i][j]);
		printf("\n");
	}

	// Displaying matB
    printf("\nMatrix B:\n");
	for (int i = 0; i < MAX; i++) {
		for (int j = 0; j < MAX; j++)
			printf("%d ", matB[i][j]);
		printf("\n");
	}

	// declaring threads
	pthread_t threads[MAX_THREAD];

	// create threads
    int* p; // for row/step number of the resultant martrix
    int a = 0;
    p = &a;
	for (int i = 0; i < MAX_THREAD; i++) {
		pthread_create(&threads[i], NULL, multi, (void*)(p));
        // pthread_join(threads[i], NULL); // calculate the thread[i]'s work first then we go for next thread
        
        /* if we join here the ans will be correct but thread won't run concurrently 
           our goal is to run thread concurrently rather than actual result */
        
        sleep(1); // to avoid wrong result. to gain synchronization

        (*p)++;  // increase row/step number
	}

    // join
    for (int i=0; i<MAX_THREAD; i++)
        pthread_join(threads[i], NULL);

	// result matrix
	printf("Multiplication of A and B:\n");
	for (int i = 0; i < MAX; i++) {
		for (int j = 0; j < MAX; j++)
			printf("%d ", matC[i][j]);
		printf("\n");
	}
	return 0;
}
