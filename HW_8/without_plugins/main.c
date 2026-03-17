#include <stdio.h>
#include "my_func.h"
int main()
{
    while(1)
    {
        int menu, a, b;
        printf("Vibor:\n1) Add\n2) Sub\n3) Mul\n4) Div\n5) Exit\n");
        scanf("%d", &menu);
        switch (menu)
        {
        case 1:
            printf("a = ");
            scanf("%d", &a);
            printf("b = ");
            scanf("%d", &b);
            printf("Result = %d\n", add(a, b));
            break;
        case 2:
            printf("a = ");
            scanf("%d", &a);
            printf("b = ");
            scanf("%d", &b);
            printf("Result = %d\n", sub(a, b));
            break;
        case 3:
            printf("a = ");
            scanf("%d", &a);
            printf("b = ");
            scanf("%d", &b);
            printf("Result = %d\n", mul(a, b));
            break;
        case 4:
            printf("a = ");
            scanf("%d", &a);
            printf("b = ");
            scanf("%d", &b);
            printf("Result = %d\n", div(a, b));
            break;
        case 5:
            return 0;
            break;
        default:
        printf("error");
            break;
        }
    }
}
