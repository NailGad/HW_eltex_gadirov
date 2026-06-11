#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

#define NAME "fifo"
#define MES "Hi!"
int main() {
	
	int fd;
	if(mkfifo(NAME, 0666) == -1) //создаем
	{
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}
	
	printf("создал канал\n");
	fd = open(NAME,O_WRONLY); //открываем канал
	if(fd == -1)
	{
		perror("open");
		unlink(NAME);
		exit(EXIT_FAILURE);
	}
	if(write(fd, MES, strlen(MES)+1) == -1) // записываем в канал 
	{
		perror("write");
		close(fd);
		unlink(NAME);
		exit(EXIT_FAILURE);
		
	}
	printf("записано!\n");
	close(fd); //закрываем канал
	
    return 0;
}