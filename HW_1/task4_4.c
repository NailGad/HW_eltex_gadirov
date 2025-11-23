#include "stdio.h"

void print_binary(unsigned int n) {
    if (n > 1) {
        print_binary(n >> 1);
    }
    (n & 1) ? printf("1"):printf("0");
}

int main() {
    int number;
    unsigned char new_byte;
    
    printf("(0-4294967295): ");
    scanf("%d", &number);
    printf("(0-255): ");
    scanf("%hhu", &new_byte);

    print_binary(number);
    printf("\n");

    number = (number & ~(0xFF << 16)) | (new_byte << 16);
    print_binary(number);
    printf("\n");

    printf("=%d\n", number);
    return 0;
}

