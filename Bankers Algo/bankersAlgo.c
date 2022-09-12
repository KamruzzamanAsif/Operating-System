#include <stdio.h>
#include <stdlib.h>

#define n 5 // total processes
#define m 4 // total resources

/* // Book's example queston
int allocation[n][m] = {
    {0, 1, 0},
    {2, 0, 0},
    {3, 0, 2}, 
    {2, 1, 1},
    {0, 0, 2}
};

int maximum[n][m] = {
    {7, 5, 3},
    {3, 2, 2},
    {9, 0, 2}, 
    {2, 2, 2},
    {4, 3, 3}
};

int available[m] = {3, 3, 2};
*/

// for 8.3 exercise question
int allocation[n][m] = {
    {0, 0, 1, 2},
    {1, 0, 0, 0},
    {1, 3, 5, 4}, 
    {0, 6, 3, 2},
    {0, 0, 1, 4}
};

int maximum[n][m] = {
    {0, 0, 1, 2},
    {1, 7, 5, 0},
    {2, 3, 5, 6}, 
    {0, 6, 5, 2},
    {0, 6, 5, 6}
};

int available[m] = {1, 5, 2, 0};


int main(void){
    int finished[n];
    for(int i=0; i<n; i++) finished[i] = 0;

    int need[n][m];
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }


    int sequence[n], sequence_index = 0;
    for(int l=0; l<m*m; l++){
        for(int i=0; i<n; i++){
            if(finished[i]==0){
                int yes = 1;
                for(int j=0; j<m; j++){
                    if(need[i][j] > available[j])
                        yes = 0;
                }

                if(yes == 1){
                    sequence[sequence_index++] = i;

                    //update resource
                    for(int k=0; k<m; k++) 
                        available[k] += allocation[i][k];

                    finished[i] = 1;
                }
            }
        }
    }


    // check for safe sequence
    int count = 0;
    for(int i=0; i<n; i++){
        if(finished[i] == 1) count++;
    }

    if(count==n){
        printf("There is a safe sequence\nThe sequence is: ");
        for(int i=0; i<n; i++)
            printf("%d ", sequence[i]);
    }
    else{
        printf("There no safe sequence.\n");
    }

    return 0;
}