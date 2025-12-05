#include "stdio.h"
#define N 5
int main()

{
    int arr[N][N] = {0};
    int val = 1;

    int zads = (N+1)/2;
    for (int zad = 0; zad < zads; zad++)
    {
        for(int i=zad;i<N-zad;i++)
        {
            arr[zad][i] = val;
            val++;
        }
        for(int i=zad+1;i<N-zad;i++)
        {
            arr[i][N-zad-1] = val;
            val++;
        }
        for(int i=N-zad-1;i>zad;i--)
        {
            arr[N-zad-1][i-1] = val;
            val++;
        }
        for(int i=N-zad-1;i>zad+1;i--)
        {
            arr[i-1][zad] = val;
            val++;
        }
        
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%3d", arr[i][j]);
        }
        printf("\n");
    }
}