#ifndef CLIENT_FUN_H
#define CLIENT_FUN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <ncurses.h>

#include "shared.h"

#define SHM_NAME "/chat_shm"
#define SEM_ACCESS "/chat_sem_access"
#define SEM_SERVER "/chat_sem_server"

extern ChatData *shared;
extern sem_t *sem_access;
extern sem_t *sem_server;
extern sem_t *my_sem;

extern WINDOW *chat_win;
extern WINDOW *users_win;
extern WINDOW *input_win;

extern char my_name[NAME_LEN];
extern int running;
extern int my_client_id;

extern pthread_mutex_t ncurses_mutex;

void init_ncurses();
void cleanup_ncurses();
void update_users_list();
void update_chat();
void *receive_thread_func(void *arg);
void *send_thread_func(void *arg);
void client_add_message(const char *sender, const char *text, int is_system);

#endif