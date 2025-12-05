#include "stdio.h"
#define N 5
int main()

{
    int arr[N][N];
    // for (int i = 0; i < N; i++)
    // {
    //     for (int j = 0; j < N; j++)
    //     {
    //         arr[i][j] = 1;
    //     }
    // }
    // for (int i = 0; i < N; i++)
    // {
    //     for (int j = 0; j < N - i - 1; j++)
    //     {
    //         arr[i][j] = 0;
    //     }
    // }
    // for (int i = 0; i < N; i++)
    // {
    //     for (int j = 0; j < N; j++)
    //     {
    //         printf("%2d", arr[i][j]);
    //     }
    //     printf("\n");
    // }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            arr[i][j] = 1;
            if( j < N - i - 1)
            {
                arr[i][j] = 0;
            }
            printf("%2d", arr[i][j]);
        }
        printf("\n");
    }

}