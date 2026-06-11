#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	if(argc != 2){
		fprintf(stderr, "Использование: %s <PID_получателя>\n", argv[0]);
        exit(EXIT_FAILURE);
	}
	pid_t pid = atoi(argv[1]);
	printf("[ОТПРАВИТЕЛЬ] PID: %d Отправляю SIGUSR1 процессу %d...\n", getpid(), pid);

    if (kill(pid, SIGUSR1) == -1) { // отправляем сигнал SIGUSR1 процессу с PID pid
        perror("kill");
        exit(EXIT_FAILURE);
    }

    printf("[ОТПРАВИТЕЛЬ] Сигнал SIGUSR1 успешно отправлен\n");

    return 0;
}