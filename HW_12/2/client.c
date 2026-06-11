#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

#define NAME "fifo"

int main() {
	int fd;
	char mes[100]; 
	fd = open(NAME,O_RDONLY); //открывааем канал 
	
	if(fd == -1)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
	ssize_t count = read(fd, mes, sizeof(mes)-1); //читаем с канала
	if(count == -1)
	{
		perror("read");
		exit(EXIT_FAILURE);
	}
	mes[count] = '\0';
	printf("прочитсно: %s\n", mes);
	close(fd); //зыкрываем канал
	if(unlink(NAME) == -1) //удаляем канал
	{
		perror("unlink");
		exit(EXIT_FAILURE);
	}
	
    return 0;
}