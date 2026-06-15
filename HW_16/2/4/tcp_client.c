#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 64

int main() {
  int sock;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];

  // Создаём TCP сокет
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    return 1;
  }

  // Настраиваем адрес сервера
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

  // Подключаемся к серверу
  if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("connect");
    return 1;
  }

  // Получаем время от сервера
  recv(sock, buffer, BUFFER_SIZE, 0);
  printf("[TCP] Текущее время: %s\n", buffer);

  close(sock);
  return 0;
}