#include "fun.h"

int main() {
  int server_fd;
  struct sockaddr_in server_addr;
  sem_init(&free_worker_sem, 0, POOL_SIZE);
  // Создаём пул
  init_pool();

  // Даём потокам время на запуск
  sleep(1);

  // Настройка сервера
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket");
    return 1;
  }

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

  printf("Сервер слушает порт %d...\n", PORT);

  int client_count = 0;

  while (1) {
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
      perror("accept");
      continue;
    }

    client_count++;

    printf("\n[Клиент #%d] Пришёл\n", client_count);

    // проверяем, есть ли свободные воркеры
    int test;
    sem_getvalue(&free_worker_sem, &test);
    if (test == 0) {
      printf("\nВСЕ ВОРОКИ ЗАНЯТЫ\n");
    }
    // ожидаем освобождения воркера
    sem_wait(&free_worker_sem);

    // Ищем свободного воркера
    int worker_id = find_free_worker();

    // Назначаем клиента воркеру
    pthread_mutex_lock(&workers[worker_id].mutex);
    workers[worker_id].client_fd = client_fd;
    workers[worker_id].client_num = client_count;
    pthread_cond_signal(&workers[worker_id].cond);
    pthread_mutex_unlock(&workers[worker_id].mutex);
  }

  server_running = 0;
  close(server_fd);

  return 0;
}