#include "func.h"

//функция для парсинга команд и разбивка из на под строки
void parsing(char *cmds[MAX_CMDS][MAX_ARGS], int *cmd_count, char *input) {

  // сохраняем все команды во временный массив строк
  char *cmd_save[MAX_CMDS];
  char *cmd_token = strtok(input, "|");

  while (cmd_token != NULL && *cmd_count < MAX_CMDS) {
    while (*cmd_token == ' ')
      cmd_token++;
    char *end = cmd_token + strlen(cmd_token) - 1;
    while (end > cmd_token && *end == ' ')
      *end-- = '\0';

    cmd_save[*cmd_count] = cmd_token;
    (*cmd_count)++;
    cmd_token = strtok(NULL, "|");
  }

  if (*cmd_count == 0)
    return;

  for (int i = 0; i < *cmd_count; i++) {
    int arg_count = 0;
    char *arg_token = strtok(cmd_save[i], " ");
    while (arg_token != NULL && arg_count < MAX_ARGS - 1) {
      cmds[i][arg_count++] = arg_token;
      arg_token = strtok(NULL, " ");
    }
    cmds[i][arg_count] = NULL;
  }
}
//функция для выполнения одной команды без канала
void only_one_cmd(char *args[MAX_ARGS]) {
  pid_t pid = fork();

  if (pid == -1) {
    perror("fork");
    return;
  }

  if (pid == 0) {
    execvp(args[0], args);
    perror("execvp");
    exit(EXIT_FAILURE);
  } else {
    wait(NULL);
  }
}

//выполнение несколькоих команд с помощью каналов
void pipes(char *cmds[MAX_CMDS][MAX_ARGS], int cmd_count) {
  int pipes[MAX_CMDS - 1][2];
  pid_t pids[MAX_CMDS];

  // Создаём каналы
  for (int i = 0; i < cmd_count - 1; i++) {
    if (pipe(pipes[i]) == -1) {
      perror("pipe");
      return;
    }
  }

  // Запускаем все команды
  for (int i = 0; i < cmd_count; i++) {
    pids[i] = fork();

    if (pids[i] == -1) {
      perror("fork");
      return;
    }

    if (pids[i] == 0) {
      // Перенаправляем stdin
      if (i > 0) {
        dup2(pipes[i - 1][0], STDIN_FILENO);
      }
      // Перенаправляем stdout
      if (i < cmd_count - 1) {
        dup2(pipes[i][1], STDOUT_FILENO);
      }

      // Закрываем ВСЕ дескрипторы каналов
      for (int j = 0; j < cmd_count - 1; j++) {
        close(pipes[j][0]);
        close(pipes[j][1]);
      }

      execvp(cmds[i][0], cmds[i]);
      perror("execvp");
      exit(EXIT_FAILURE);
    }
  }

  // Закрываем каналы в родителе
  for (int i = 0; i < cmd_count - 1; i++) {
    close(pipes[i][0]);
    close(pipes[i][1]);
  }

  // Ждём завершения всех потомков
  for (int i = 0; i < cmd_count; i++) {
    waitpid(pids[i], NULL, 0);
  }
}