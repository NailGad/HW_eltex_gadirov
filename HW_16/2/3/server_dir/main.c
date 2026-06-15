#include "fun.h"

int main() {
  int server_fd;
  struct sockaddr_in server_addr;
  pthread_t workers[POOL_SIZE];

  // СОЗДАЁМ ОЧЕРЕДЬ СООБЩЕНИЙ
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;

  // Удаляем старую очередь (если есть)
  mq_unlink(QUEUE_NAME);

  // Создаём новую очередь
  msg_queue = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
  if (msg_queue == (mqd_t)-1) {
    perror("mq_open");
    return 1;
  }

  // СОЗДАЁМ ПУЛ ПОТРЕБИТЕЛЕЙ
  for (int i = 0; i < POOL_SIZE; i++) {
    int *id = malloc(sizeof(int));
    *id = i;
    pthread_create(&workers[i], NULL, worker_function, id);
  }
  printf("Пул из %d потоков-потребителей создан\n", POOL_SIZE);

  // НАСТРОЙКА СЕРВЕРА
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("bind");
    return 1;
  }

  if (listen(server_fd, 10) < 0) {
    perror("listen");
    return 1;
  }

  // принимает клиентов и кладёт в очередь
  int client_count = 0;

  while (1) {
    client_info client;
    client.client_fd = accept(server_fd, NULL, NULL);
    if (client.client_fd < 0)
      continue;

    client_count++;
    client.client_id = client_count;
    printf("\n[Клиент #%d] Пришёл в очередь\n", client_count);

    // Кладём файловый дескриптор в очередь сообщений
    if (mq_send(msg_queue, (const char *)&client, MAX_MSG_SIZE, 0) == -1) {
      perror("mq_send");
      close(client.client_fd);
      continue;
    }
  }

  // ОЧИСТКА
  mq_close(msg_queue);
  mq_unlink(QUEUE_NAME);
  close(server_fd);

  return 0;
}