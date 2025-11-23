#include "stdio.h"
int count=0;
void print_binary(unsigned int n ) {
    if (n > 1) {
        print_binary(n >> 1);
    }
    if(n & 1)
        count += 1;

}
int main() {
    unsigned int number;
    
    scanf("%u", &number);
    print_binary(number);
    printf("%d", count);
}