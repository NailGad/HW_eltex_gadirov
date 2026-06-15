#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "./mysocket"
#define BUFFER_SIZE 32

int main() {
  // создание сокета
  int server_fd, client_fd;
  struct sockaddr_un server_sock;
  char buffer[BUFFER_SIZE];

  unlink(SOCKET_PATH);
  // инициализация структуры адреса сокета
  memset(&server_sock, 0, sizeof(server_sock));
  server_sock.sun_family = AF_LOCAL;
  strcpy(server_sock.sun_path, SOCKET_PATH);

  // привязка сокета к адресу
  server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket");
    return 1;
  }

  if (bind(server_fd, (struct sockaddr *)&server_sock, sizeof(server_sock)) <
      0) {
    perror("bind");
    return 1;
  }
  // ожидание подключений
  if (listen(server_fd, 5) < 0) {
    perror("listen");
    return 1;
  }

  // принятие подключения
  client_fd = accept(server_fd, NULL, NULL);
  if (client_fd < 0) {
    perror("accept");
    return 1;
  }
  // чтение данных от клиента
  recv(client_fd, buffer, BUFFER_SIZE, 0);
  printf("%s", buffer);

  strcpy(buffer, "hi!\n");
  send(client_fd, buffer, strlen(buffer) + 1, 0);
  // отправка данных клиенту
  close(client_fd);
  close(server_fd);
  unlink(SOCKET_PATH);
  return 0;
}