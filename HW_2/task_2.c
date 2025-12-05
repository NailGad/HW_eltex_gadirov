#include "stdio.h"
#define N 5
int main()

{
    int arr[N] = {1,2,3,4,5};
    for(int i=0;i<N/2;i++)
    {
        int temp;
        temp = arr[i];
        arr[i] = arr[N-i-1];
        arr[N-i-1] = temp;
    }
    for(int i=0;i<N;i++)
    {
        printf("%d ",arr[i]); 
    }
}