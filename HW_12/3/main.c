#include "func.h"

int main() {
  char input[INPUT_SIZE];

  while (1) {
    printf("pipe_shell> ");
    //ввод данных
    if (fgets(input, sizeof(input), stdin) == NULL)
      break;
    input[strcspn(input, "\n")] = '\0';
    if (strcmp(input, "exit") == 0)
      break;
    if (strlen(input) == 0)
      continue;

    //парсинг
    char *cmds[MAX_CMDS][MAX_ARGS];
    int cmd_count = 0;
    parsing(cmds, &cmd_count, input);

    if (cmd_count == 0)
      continue;
    
    if (cmd_count == 1) {
      only_one_cmd(cmds[0]); //запуск комнады если она одна
    } else {
      pipes(cmds, cmd_count); //запуск несколько команд
    }
  }

  return 0;
}