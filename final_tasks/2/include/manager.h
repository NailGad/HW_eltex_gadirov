#ifndef MANAGER_H
#define MANAGER_H

#include "common.h"

extern Driver drivers[MAX_DRIVERS];
extern int drivers_count;
extern int server_running;
extern int pipe_fd[2];

void setup_signals(void);
int find_driver(pid_t pid);
void create_driver(const char *name);
void send_task(pid_t pid, int timer);
void get_status(pid_t pid);
void get_drivers(void);
void cleanup_all(void);

#endif