#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main()
{
	
	pid_t pid1 = fork();
	
	if(pid1 == 0)
	{
		
		pid_t pid3 = fork();
		if(pid3 == 0)
		{
			printf("child3 %d %d\n", getpid(), getppid());
			exit(33);
		}
		else if(pid3 > 0)
		{
			pid_t pid4 = fork();
			if(pid4 == 0)
			{
				printf("child4 %d %d\n", getpid(), getppid());
				exit(44);

			}
			else if(pid4 >0)
			{
				printf("child1: wait 3 and 4 %d %d\n", getpid(), getppid());

				int status3, status4;
				pid_t wait3, wait4;
				wait3 = wait(&status3);
				wait4 = wait(&status4);
				if(WEXITSTATUS(status3))
				{
					printf("child3 stoped : %d\n", WEXITSTATUS(status3));
				}
				if(WEXITSTATUS(status4))
				{
					printf("child4 stoped : %d\n", WEXITSTATUS(status4));
				}

				exit(11);
			}
		}
	}
	else if (pid1>0)
	{
		
		pid_t pid2 = fork();
		
		if(pid2 == 0)
		{
			pid_t pid5 = fork();
			if(pid5 == 0)
			{
				printf("child5 %d %d\n", getpid(), getppid());
				exit(55);

			}
			else if(pid5 > 0)
			{
				printf("child2: wait 5 %d %d\n", getpid(), getppid());

				int status5;
				pid_t wait5;
				wait5 = wait(&status5);
				if(WEXITSTATUS(status5))
				{
					printf("child5 stoped : %d\n", WEXITSTATUS(status5));
				}
				exit(22);

			}
		}
		else if(pid2>0)
		{
			printf("dad: wait 1 and 2 %d %d\n", getpid(), getppid());

			int status1, status2;
			pid_t wait1, wait2;
			wait1 = wait(&status1);
			wait2 = wait(&status2);
			if(WEXITSTATUS(status1))
			{
				
				printf("child1 stoped : %d\n", WEXITSTATUS(status1));
			}
			if(WEXITSTATUS(status2))
			{
				printf("child2 stoped : %d\n", WEXITSTATUS(status2));
			}			
		}
	}
	else{
		printf("error");
	}
	

	
}	