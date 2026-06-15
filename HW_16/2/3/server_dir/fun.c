#include "fun.h"
#include <unistd.h>

// Глобальная очередь сообщений
mqd_t msg_queue;

// ОБСЛУЖИВАНИЕ КЛИЕНТА
void handle_client(int client_fd, int worker_id, int client_id) {
  char buffer[BUF_SIZE];
  time_t rawtime;

  printf("[Worker %d] НАЧАЛ обработку клиента [#%d]\n", worker_id, client_id);

  // имитация работы
  sleep(1);
  time(&rawtime);
  ctime_r(&rawtime, buffer);
  buffer[strcspn(buffer, "\n")] = '\0';

  send(client_fd, buffer, strlen(buffer) + 1, 0);
  close(client_fd);

  printf("[Worker %d] ЗАКОНЧИЛ обработку клиента [#%d]\n", worker_id,
         client_id);
}

// ВОРКЕР
void *worker_function(void *arg) {
  int worker_id = *(int *)arg;
  free(arg);
  client_info client;

  while (1) {
    // читаем из очереди сообщений
    if (mq_receive(msg_queue, (char *)&client, MAX_MSG_SIZE, NULL) == -1) {
      perror("mq_receive");
      continue;
    }

    printf("[Worker %d] Забрал клиента [#%d] из очереди\n", worker_id,
           client.client_id);
    handle_client(client.client_fd, worker_id, client.client_id);
  }

  return NULL;
}
