#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>        
#include <fcntl.h>           

#define SHM_NAME "/myshm"
#define SEM_NAME "/mysem"
#define SHM_SIZE 1024
int main() {
    int shm_d = shm_open(SHM_NAME, O_RDWR, 0666);
    sem_t *sem_d = sem_open(SEM_NAME, 0);
    if(shm_d == -1) {
        perror("shm_open");
        return 1;
    }
    char *str = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_d, 0);
    if(str == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    
    printf("Client reads: %s\n", str);
    strcpy(str, "Hello!");

    sem_post(sem_d);
    
    munmap(str, SHM_SIZE);
    sem_close(sem_d);
    close(shm_d);
    sem_unlink(SEM_NAME);
    shm_unlink(SHM_NAME);
	return 0;
}