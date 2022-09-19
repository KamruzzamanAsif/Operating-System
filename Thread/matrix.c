#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

// size of matrix
#define MAX 9

// number of threads
#define MAX_THREAD 9

int matA[COL][COL];
int matB[COL][COL];
int matC[COL][COL];
//int i =0;

void* multi(void* arg)
{
    int i = *(int*) arg; // i is the step/row num
    // printf("i value is :%d \n", i);
	
	for (int j = 0; j < COL; j++)
	    for (int k = 0; k < COL; k++)
		    matC[i][j] += matA[i][k] * matB[k][j];
}


int main()
{
	// Generating random values in matA and matB
	for (int i = 0; i < COL; i++) {
		for (int j = 0; j < COL; j++) {
			matA[i][j] = 1;//rand() % 10;
			matB[i][j] = 1;//rand() % 10;
		}
	}

	// Displaying matA
	printf("\nMatrix A:\n");
	for (int i = 0; i < COL; i++) {
		for (int j = 0; j < COL; j++)
			printf("%d ", matA[i][j]);
		printf("\n");
	}

	// Displaying matB
    printf("\nMatrix B:\n");
	for (int i = 0; i < COL; i++) {
		for (int j = 0; j < COL; j++)
			printf("%d ", matB[i][j]);
		printf("\n");
	}

	// declaring threads
	pthread_t threads[MAX_THREAD];

	// create threads
    
	for (int i = 0; i < MAX_THREAD; i++) {
		int* tid; // for row/step number of the resultant martrix
		tid = (int *) malloc( sizeof(int) );
   		*tid = i;

		pthread_create(&threads[i], NULL, multi, (void*)(tid));
	}

    // join
    for (int i=0; i<MAX_THREAD; i++)
        pthread_join(threads[i], NULL);

	// result matrix
	printf("Multiplication of A and B:\n");
	for (int i = 0; i < COL; i++) {
		for (int j = 0; j < COL; j++)
			printf("%d ", matC[i][j]);
		printf("\n");
	}
	return 0;
}
