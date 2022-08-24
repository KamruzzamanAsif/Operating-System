# include <stdio.h>

int main(){
    int *p;
    int a = 0;
    p = &a;
    (*p)++;
    printf("%d ", *p);
}