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
  socklen_t addr_len = sizeof(server_addr);

  // Создаём UDP сокет
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    return 1;
  }

  // Настраиваем адрес сервера
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

  // Отправляем запрос на сервер (UDP не требует connect)
  strcpy(buffer, "TIME");
  sendto(sock, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&server_addr,
         addr_len);
  printf("[UDP] Запрос отправлен\n");

  // Получаем ответ
  recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr,
           &addr_len);
  printf("[UDP] Текущее время: %s\n", buffer);

  close(sock);
  return 0;
}