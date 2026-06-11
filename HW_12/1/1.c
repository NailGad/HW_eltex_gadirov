#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
int main() {
	int pi[2]; //создаем массив для канала
	if(pipe(pi) == -1) // сам канал 
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	pid_t pid = fork(); 

	if(pid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if(pid == 0) //дочерний
	{
		char buf[10]; 
		close(pi[1]); //закрываем канал на запись
		ssize_t	count = read(pi[0], buf, sizeof(buf)-1); //только читаем с канала
		if(count > 0)
		{
			buf[count] = '\0';
			printf("дочерний процесс прочитал %s\n",buf);
		}
		else {
			perror("read");
		}
		close(pi[0]); //закрываем канла на чтение
		exit(0); //завершаем прцесс
	}
	else if(pid > 0) { //родитель
		close(pi[0]); //закрывем на чтение
		char *mes = "Hi!";
		write(pi[1], mes, strlen(mes) + 1); //только записываем в канал
		close(pi[1]); //закрываем на запись
		wait(NULL); // ожидаем завершение дочернего
}
    return 0;
}