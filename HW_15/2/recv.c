#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
    sigset_t mask; // создаем маску сигналов

    printf("[БЛОКИРОВЩИК] Мой PID: %d\n", getpid());
    printf("[БЛОКИРОВЩИК] Блокирую SIGINT...\n");

    sigemptyset(&mask); // очищаем маску сигналов
    sigaddset(&mask, SIGINT); // добавляем SIGINT в маску

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) { // блокируем сигнал SIGINT
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    while (1) {
   	printf("жду смерти\n");
        pause();     }

    return 0;
}