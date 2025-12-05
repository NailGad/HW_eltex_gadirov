#include "stdio.h"
#define N 10
#define P 3

int main()
{
    char stroka[N], pod[P], end_stroki;
    int i1 = 0, i2 = 0;
    char *ptr = NULL;
    while (i1 < N)
    {
        scanf("%c", &stroka[i1]);
        i1++;
    }

    while (!scanf("%c", &end_stroki)); // это для того чтобы нашь enter не записался в строку или подстроку
    
    while (i2 < P)
    {
        scanf("%c", &pod[i2]);
        i2++;
    }

    for (int i = 0; i <= N - P; i++)
    {
        if (pod[0] == stroka[i])
        {
            int flag = 1;
            ptr = &stroka[i];
            for (int j = 0; j < P; j++)
            {
                if (pod[j] != stroka[i + j])
                {
                    flag = 0;
                    break;
                }
            }
            if (!flag)
            {
                ptr = NULL;
            }
            else
            {
                break;
            }
        }
    }

    printf("%p\n", ptr);
}
