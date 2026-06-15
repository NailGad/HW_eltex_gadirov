#include <arpa/inet.h>
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
#define NUM_THREADS 10
void *send_time(void *arg) {

  int client_fd = *(int *)arg;
  free(arg);
  char buffer[BUF_SIZE];

  time_t rawtime;
  time(&rawtime);
  ctime_r(&rawtime, buffer);
  buffer[strcspn(buffer, "\n")] = '\0';
  sleep(1);
  send(client_fd, buffer, strlen(buffer) + 1, 0);
  close(client_fd);

  return NULL;
}

int main() {
  // инициализация сокета
  int server_fd;
  struct sockaddr_in server_sock;
  // создание сокета
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket");
    return 1;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // инициализация структуры адреса сервера
  memset(&server_sock, 0, sizeof(server_sock));
  server_sock.sin_family = AF_INET;
  server_sock.sin_port = htons(PORT);
  server_sock.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, (struct sockaddr *)&server_sock, sizeof(server_sock)) <
      0) {
    perror("bind");
    return 1;
  }
  // подключение к серверу
  if (listen(server_fd, 5) < 0) {
    perror("listen");
    return 1;
  }
  while (1) {
    int *client_fd = malloc(sizeof(int));
    // ожидание подключения клиента

    *client_fd = accept(server_fd, 0, 0);
    if (*client_fd < 0) {
      perror("accept");
      return 1;
    }
    pthread_t client_thread;
    if (pthread_create(&client_thread, NULL, send_time, (void *)client_fd) !=
        0) {
      perror("pthread_create");
      close(*client_fd);
      free(client_fd);
      continue;
    }
    pthread_detach(client_thread);
  }
  close(server_fd);

  return 0;
}