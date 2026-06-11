#ifndef SERVER_FUN_H
#define SERVER_FUN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <signal.h>

#include "shared.h"

#define SHM_NAME "/chat_shm"
#define SEM_ACCESS "/chat_sem_access"
#define SEM_SERVER "/chat_sem_server"

extern ChatData *shared;
extern sem_t *sem_access;
extern sem_t *sem_server;

void cleanup(int sig);
void add_message(const char *sender, const char *text, int is_system);
void notify_all_clients();
int is_name_taken(const char *name);

#endif