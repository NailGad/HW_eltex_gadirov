#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "./mysocket"
#define BUFFER_SIZE 32

int main() {
  // инициализация структуры адреса сокета
  int client_fd;
  struct sockaddr_un server_sock;
  char buffer[BUFFER_SIZE];

  // создание сокета
  client_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (client_fd < 0) {
    perror("socket");
    return 1;
  }
  // подключение к серверу
  memset(&server_sock, 0, sizeof(server_sock));
  server_sock.sun_family = AF_LOCAL;
  strcpy(server_sock.sun_path, SOCKET_PATH);
  // подключение к серверу
  if (connect(client_fd, (struct sockaddr *)&server_sock, sizeof(server_sock)) <
      0) {
    perror("connect");
    return 1;
  }
  // отправка данных на сервер
  strcpy(buffer, "hello!\n");
  send(client_fd, buffer, strlen(buffer) + 1, 0);
  // получение ответа от сервера
  recv(client_fd, buffer, BUFFER_SIZE, 0);
  printf("%s", buffer);

  close(client_fd);
  return 0;
}