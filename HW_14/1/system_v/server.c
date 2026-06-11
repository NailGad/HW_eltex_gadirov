#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>
#define SHM_SIZE 1024
int main() {

	key_t key = ftok("shm", 1);
	if(key == -1) {
		perror("ftok");
		return 1;
	}
	int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
	if(shmid == -1) {
		perror("shmget");
		return 1;
	}
	char *str = shmat(shmid, (void*)0, 0);
	if(str == (void*)-1) {
		perror("shmat");
		return 1;
	}
	int semid = semget(key, 1, IPC_CREAT | 0666);
	if(semid == -1) {
		perror("semget");
		return 1;
	}
	semctl(semid, 0, SETVAL, 0);

	strcpy(str, "Hi!");
	struct sembuf sb = {0, -1, 0};
	semop(semid, &sb, 1);
	printf("Server reads: %s\n", str);

	shmdt(str);
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}