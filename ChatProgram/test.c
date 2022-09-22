#include <stdio.h>

int main(){
    int i = 0;
    int *p = &i;

    *p = (int)2;

    printf("%d\n", i);

    return 0;
}