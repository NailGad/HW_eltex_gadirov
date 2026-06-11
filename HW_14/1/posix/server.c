#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>        
#include <fcntl.h>

#include <semaphore.h>

#define SHM_NAME "/myshm"
#define SEM_NAME "/mysem"
#define SHM_SIZE 1024
int main() {

	int shm_d = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
	sem_t *sem_d = sem_open(SEM_NAME, O_CREAT, 0666, 0);
	if(shm_d == -1) {
		perror("open");
		return 1;
	}
    ftruncate(shm_d, SHM_SIZE);
   	char *str = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_d, 0);
    if(str == MAP_FAILED) {
		perror("mmap");
		return 1;
	}
    
	strcpy(str, "Hi!");
	sem_wait(sem_d);
	
	printf("Server reads: %s\n", str);
	munmap(str, SHM_SIZE);
	close(shm_d);
	shm_unlink(SHM_NAME);
	sem_unlink(SEM_NAME);
	return 0;
}