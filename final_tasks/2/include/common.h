#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#define MAX_DRIVERS 20
#define INPUT_SIZE 32
#define SIZE_RESPONSE 128
#define NAME_LEN 32

typedef enum {
    AVAILABLE,
    BUSY
} DriverStatus;

typedef struct {
    pid_t pid;
    int write_fd;   // менеджер → водитель
    int read_fd;    // водитель → менеджер
    DriverStatus status;
    char name[NAME_LEN];
} Driver;

#endif