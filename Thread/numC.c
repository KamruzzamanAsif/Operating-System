#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

// size of matrix
#define COL 8000
#define ROW 7000

int matA[ROW][COL];

// number of threads
int count = 0;

int main()
{

    int i, j;
    for (i = 0; i < ROW; ++i) {
        for (j = 0; j < COL; ++j) {
          matA[ i ][ j ] = 1;
        }
    }

    for (i = 0; i < ROW; ++i) {
        for (j = 0; j < COL; ++j) {
          if(matA[ i ][ j ] = 1) count++;
        }
    }

	
    
    printf("total 1 is: %d\n", count);

	return 0;
}
