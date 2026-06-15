#include "stdio.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 32
#define PORT 8080

int main() {
  // инициализация сокета
  int client_fd;
  struct sockaddr_in server_sock;
  char buffer[BUF_SIZE];
  // создание сокета
  client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd < 0) {
    perror("socket");
    return 1;
  }
  // инициализация структуры адреса сервера
  memset(&server_sock, 0, sizeof(server_sock));
  server_sock.sin_family = AF_INET;
  server_sock.sin_port = htons(PORT);
  inet_pton(AF_INET, "127.0.0.1", &server_sock.sin_addr);

  // подключение к серверу
  if (connect(client_fd, (struct sockaddr *)&server_sock, sizeof(server_sock)) <
      0) {
    perror("connect");
    return 1;
  }

  // отправка данных серверу
  strcpy(buffer, "hello!");
  send(client_fd, buffer, strlen(buffer) + 1, 0);

  recv(client_fd, buffer, BUF_SIZE, 0);
  printf("%s\n", buffer);

  close(client_fd);
  return 0;
}