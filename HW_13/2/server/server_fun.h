#pragma once
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#define SERVER_QUEUE "/chat_server"
#define MAX_CLIENTS 50
#define MAX_MSG_SIZE 1024
#define MAX_NAME_SIZE 50
#define HISTORY_FILE "chat_history.txt"

typedef struct {
  char name[MAX_NAME_SIZE];
  char queue_name[MAX_MSG_SIZE];
  int active;
} Client;

extern Client clients[MAX_CLIENTS];
extern mqd_t server_queue;

void delete_history();
void save_to_history(const char *from, const char *msg);
void broadcast(const char *msg, int skip);
void send_user_list();
void send_history(const char *client_q);
int find_client(const char *name);
void add_client(const char *name, const char *qname);
void remove_client(const char *name);
void signal_handler(int sig);