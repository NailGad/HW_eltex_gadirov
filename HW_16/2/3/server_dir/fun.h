#pragma once
#include <arpa/inet.h>
#include <fcntl.h>
#include <mqueue.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 32
#define POOL_SIZE 5
#define QUEUE_NAME "/queue"
#define MAX_MSG_SIZE sizeof(client_info)

typedef struct {
  int client_fd;
  int client_id;
} client_info;

// Глобальная очередь сообщений
extern mqd_t msg_queue;

// ОБСЛУЖИВАНИЕ КЛИЕНТА
void handle_client(int client_fd, int worker_id, int client_id);
// ВОРКЕР
void *worker_function(void *arg);