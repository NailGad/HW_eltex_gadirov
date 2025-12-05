#include "stdio.h"

int main()
{
    int prob;
    int new;
    scanf("%d", &prob);
    scanf("%d", &new);
    char *ptr = (char *)&prob;
    ptr +=2;
    *ptr = new;
    printf("%d\n", prob);
}   