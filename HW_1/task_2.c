#include "stdio.h"

void print_binary(unsigned int n) {
    if (n > 1) {
        print_binary(n >> 1);
    }
    (n & 1) ? printf("1"):printf("0");
}

int main() {
    int number;
    scanf("%d", &number);
    print_binary((unsigned int)number);
}

