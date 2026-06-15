#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 32

int main() {
  // создание сокета
  int client_fd;
  struct sockaddr_in server_sock;
  char buffer[BUFFER_SIZE];

  client_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (client_fd < 0) {
    perror("socket");
    return 1;
  }
  // инициализация структуры адреса сервера
  memset(&server_sock, 0, sizeof(server_sock));
  server_sock.sin_family = AF_INET;
  server_sock.sin_port = htons(PORT);
  if (inet_pton(AF_INET, "127.0.0.1", &server_sock.sin_addr) == 0) {
    perror("pton");
    return 1;
  }
  // отправка данных на сервер
  strcpy(buffer, "hello!");
  sendto(client_fd, buffer, strlen(buffer) + 1, 0,
         (struct sockaddr *)&server_sock, sizeof(server_sock));
  // получение ответа от сервера
  recvfrom(client_fd, buffer, BUFFER_SIZE, 0, NULL, NULL);
  printf("%s\n", buffer);
  // закрытие сокета
  close(client_fd);
  return 0;
}