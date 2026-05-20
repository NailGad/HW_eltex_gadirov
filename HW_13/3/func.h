#ifndef FUNC_H
#define FUNC_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CMDS 10
#define MAX_ARGS 20
#define INPUT_SIZE 256
//функция для парсинга команд и разбивка из на под строки
void parsing(char *cmds[MAX_CMDS][MAX_ARGS], int *cmd_count, char *input);
//функция для выполнения одной команды без канала
void only_one_cmd(char *args[MAX_ARGS]);
//выполнение несколькоих команд с помощью каналов
void pipes(char *cmds[MAX_CMDS][MAX_ARGS], int cmd_count);
#endif