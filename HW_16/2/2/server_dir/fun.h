#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 32
#define POOL_SIZE 5

typedef struct {
  pthread_t thread;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  int client_fd;
  int client_num;
  int is_busy;
  int worker_id;
} worker_t;

extern worker_t workers[POOL_SIZE];
extern volatile int server_running;
extern sem_t free_worker_sem;

void handle_client(int client_fd, int worker_id, int client_num);

void *worker_function(void *arg);

void init_pool();

int find_free_worker();
