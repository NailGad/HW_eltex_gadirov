#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "./mysocket"
#define CLIENT_PATH "./myclient"

#define BUFFER_SIZE 32

int main() {
  // инициализация структуры адреса сокета
  int client_fd;
  char buffer[BUFFER_SIZE];
  struct sockaddr_un server_sock, client_sock;

  unlink(CLIENT_PATH);
  // создание сокета
  client_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
  if (client_fd < 0) {
    perror("client_fd");
    return 1;
  }
  // инициализация структуры адреса сокета
  memset(&server_sock, 0, sizeof(server_sock));
  server_sock.sun_family = AF_LOCAL;
  strcpy(server_sock.sun_path, SOCKET_PATH);
  // инициализация структуры адреса клиента
  memset(&client_sock, 0, sizeof(client_sock));
  client_sock.sun_family = AF_LOCAL;
  strcpy(client_sock.sun_path, CLIENT_PATH);

  // привязка сокета к адресу клиента
  if (bind(client_fd, (struct sockaddr *)&client_sock, sizeof(client_sock)) <
      0) {
    perror("bind");
    return 1;
  }

  // отправка данных на сервер
  strcpy(buffer, "hello!");
  if (sendto(client_fd, buffer, strlen(buffer) + 1, 0,
             (struct sockaddr *)&server_sock, sizeof(server_sock)) < 0) {
    perror("sendto");
    return 1;
  }
  // получение ответа от сервера
  recvfrom(client_fd, buffer, BUFFER_SIZE, 0, NULL, NULL);
  printf("%s\n", buffer);
  // закрытие сокета и удаление пути
  close(client_fd);
  unlink(CLIENT_PATH);
  return 0;
}