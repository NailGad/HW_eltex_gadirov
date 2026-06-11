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
	int shmid = shmget(key, SHM_SIZE, 0666);
	if(shmid == -1) {
		perror("shmget");
		return 1;
	}
	char *str = shmat(shmid, (void*)0, 0);
	if(str == (void*)-1) {
		perror("shmat");
		return 1;
	}

	int semid = semget(key, 1, 0666);
	if(semid == -1) {
		perror("semget");
		return 1;
	}
	
	printf("Client reads: %s\n", str);
	strcpy(str, "Hello!");
	struct sembuf sb = {0, 1, 0};
	semop(semid, &sb, 1);


	shmdt(str);
	return 0;
}