#include "fun.h"

worker_t workers[POOL_SIZE];
volatile int server_running = 1;
sem_t free_worker_sem;

void handle_client(int client_fd, int worker_id, int client_num) {
  char buffer[BUF_SIZE];
  time_t rawtime;

  printf("\n[Worker %d] НАЧАЛ обработку клиента %d\n", worker_id, client_num);

  // Имитируем работу
  sleep(1);
  time(&rawtime);
  ctime_r(&rawtime, buffer);
  buffer[strcspn(buffer, "\n")] = '\0';

  send(client_fd, buffer, strlen(buffer) + 1, 0);
  close(client_fd);

  printf("\n[Worker %d] ЗАКОНЧИЛ обработку клиента %d\n", worker_id,
         client_num);
}

void *worker_function(void *arg) {
  worker_t *worker = (worker_t *)arg;

  while (server_running) {
    pthread_mutex_lock(&worker->mutex);

    // Ждём, пока назначат клиента
    while (worker->client_fd == -1 && server_running) {
      printf("Worker %d спит, ждёт клиента\n", worker->worker_id);
      pthread_cond_wait(&worker->cond, &worker->mutex);
    }

    if (!server_running) {
      pthread_mutex_unlock(&worker->mutex);
      break;
    }

    int client_fd = worker->client_fd;
    worker->is_busy = 1;
    pthread_mutex_unlock(&worker->mutex);

    // Обрабатываем клиента
    handle_client(client_fd, worker->worker_id, worker->client_num);

    // Освобождаем воркера
    pthread_mutex_lock(&worker->mutex);
    worker->client_fd = -1;
    worker->is_busy = 0;
    pthread_mutex_unlock(&worker->mutex);

    // уведомляем слушащий сервер об освобождении воркера
    sem_post(&free_worker_sem);
  }

  return NULL;
}

void init_pool() {
  for (int i = 0; i < POOL_SIZE; i++) {
    workers[i].worker_id = i;
    workers[i].client_fd = -1;
    workers[i].is_busy = 0;
    pthread_mutex_init(&workers[i].mutex, NULL);
    pthread_cond_init(&workers[i].cond, NULL);
    pthread_create(&workers[i].thread, NULL, worker_function, &workers[i]);
  }
  printf("Пул из %d потоков создан\n", POOL_SIZE);
}

int find_free_worker() {
  for (int i = 0; i < POOL_SIZE; i++) {
    pthread_mutex_lock(&workers[i].mutex);

    // Проверяем, свободен ли воркер
    if (workers[i].client_fd == -1 && workers[i].is_busy == 0) {
      workers[i].is_busy = 1;
      pthread_mutex_unlock(&workers[i].mutex);
      printf("  Выбран Worker %d (СВОБОДЕН)\n", i);
      return i;
    }

    pthread_mutex_unlock(&workers[i].mutex);
  }

  return -1; // Все заняты
}