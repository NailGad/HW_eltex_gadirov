#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "./mysocket"
#define BUFFER_SIZE 32
int main() {
  // создание и привязка сокета
  int server_fd;
  char buffer[BUFFER_SIZE];
  struct sockaddr_un server_sock, client_sock;
  socklen_t sock_size = sizeof(client_sock);

  unlink(SOCKET_PATH);

  // создание сокета
  server_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
  if (server_fd < 0) {
    perror("fd");
    return 1;
  }
  // инициализация структуры адреса сокета
  memset(&server_sock, 0, sizeof(server_sock));
  server_sock.sun_family = AF_LOCAL;
  strcpy(server_sock.sun_path, SOCKET_PATH);
  // привязка сокета к адресу
  if (bind(server_fd, (struct sockaddr *)&server_sock, sizeof(server_sock)) <
      0) {
    perror("bind");
    return 1;
  }
  // ожидание входящего сообщения
  recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_sock,
           &sock_size);
  printf("%s\n", buffer);
  // отправка ответа клиенту
  strcpy(buffer, "hi!");
  sendto(server_fd, buffer, strlen(buffer) + 1, 0,
         (struct sockaddr *)&client_sock, sock_size);
  // закрытие сокета и удаление файла сокета
  close(server_fd);
  unlink(SOCKET_PATH);
  return 0;
}
