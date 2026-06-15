#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 32

int main() {
  // инициализация сокета
  int server_fd, client_fd;
  struct sockaddr_in server_sock;
  char bufer[BUF_SIZE];
  // создание сокета
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket");
    return 1;
  }
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
  // инициализация структуры адреса сервера
  // подключение к серверу
  if (listen(server_fd, 5) < 0) {
    perror("listen");
    return 1;
  }
  // ожидание подключения клиента
  client_fd = accept(server_fd, 0, 0);
  if (client_fd < 0) {
    perror("accept");
    return 1;
  }
  // чтение данных от клиента
  recv(client_fd, bufer, BUF_SIZE, 0);
  printf("%s\n", bufer);
  // отправка данных клиенту
  strcpy(bufer, "hi!");
  send(client_fd, bufer, strlen(bufer) + 1, 0);

  close(server_fd);
  close(client_fd);
  return 0;
}