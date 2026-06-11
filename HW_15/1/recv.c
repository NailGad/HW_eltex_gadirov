#include <bits/types/siginfo_t.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


void siguser1(int sig, siginfo_t *info, void* context)
{
	printf("\n я принял сигнал от %d \n",info->si_pid);
}

int main(int argc, char *argv[]) {
	struct sigaction sa;
    sa.sa_sigaction = siguser1;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&sa.sa_mask);  

    if(sigaction(SIGUSR1, &sa, NULL) == -1)
    {
   		perror("registr signal");
    }

    printf("[ПОЛУЧАТЕЛЬ] Мой PID: %d Ожидаю сигнал SIGUSR1...\n", getpid());

    while (1) {
        pause(); 
    }
    return 0;
}