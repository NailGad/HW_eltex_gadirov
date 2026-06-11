#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
    sigset_t mask;
    int sig;
    printf("[БЛОКИРОВЩИК] Мой PID: %d\n", getpid());
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if(sigwait(&mask, &sig) == -1) { // ожидаем сигнал SIGUSR1
            perror("sigwait");
            exit(EXIT_FAILURE);
        }
        printf("[БЛОКИРОВЩИК] Получен сигнал %d\n", sig); // выводим полученный сигнал
    }

    return 0;
}