#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main()
{
	
	pid_t pid = fork();
	
	if(pid == 0)
	{
		
		printf("child %d %d\n", getpid(), getppid());
		exit(111);
	}
	else if (pid > 0)
	{
		
		int status;
		pid_t r = wait(&status);
		printf("dad %d %d %d\n", getpid(), getppid(), WEXITSTATUS(status));

	}
	else{
		printf("error");
	}
	
	
}	