#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
    sigset_t mask;

    printf("[БЛОКИРОВЩИК] Мой PID: %d\n", getpid());
    printf("[БЛОКИРОВЩИК] Блокирую SIGINT...\n");

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    while (1) {
   	printf("жду смерти\n");
        pause();     }

    return 0;
}