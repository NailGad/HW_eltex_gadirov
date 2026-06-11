#pragma once
#include <fcntl.h>
#include <mqueue.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define SERVER_QUEUE "/chat_server"
#define MAX_MSG_SIZE 1024
#define MAX_NAME_SIZE 50

extern mqd_t server_q, client_q;
extern char client_qname[100];
extern char username[MAX_NAME_SIZE];
extern pthread_t recv_thread, send_thread;

extern WINDOW *chat_win, *users_win, *input_win;
extern pthread_mutex_t win_mutex;

void send_msg(const char *cmd, const char *data);
void update_chat(const char *msg);
void update_users(const char *data);
void *receive_thread_func(void *arg);
void *send_thread_func(void *arg);
void init_ncurses();
void cleanup();
