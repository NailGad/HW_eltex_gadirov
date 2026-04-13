#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ARGUM_SIZE 20
#define INPUT_SIZE 100

int main() {
  char input[INPUT_SIZE];
  char *argum[ARGUM_SIZE];
  char *token;

  while (1) {
    printf("nail)_(bash> ");
    if (fgets(input, sizeof(input), stdin) == NULL ||
        strcmp(input, "exit\n") == 0) {
      break;
    }
    input[strcspn(input, "\n")] = '\0';
    if (strlen(input) == 0) {
      continue;
    }
    token = strtok(input, " ");
    int i = 0;
    while (token != NULL && i < ARGUM_SIZE - 1) {
      argum[i++] = token;
      token = strtok(NULL, " ");
    }
    argum[i] = NULL;

    pid_t pid = fork();

    if (pid == 0) {
      if (execvp(argum[0], argum) == -1) {
        perror("error");
        exit(EXIT_FAILURE);
      }
    } else if (pid > 0) {
      wait(NULL);
    } else {
      perror("fork failed");
    }
  }
}