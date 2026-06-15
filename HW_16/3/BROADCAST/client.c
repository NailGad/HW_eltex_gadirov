#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 7777
#define BROADCAST_ADDR "255.255.255.255"

int main() {
  int sock;
  struct sockaddr_in local_addr, sender_addr;
  socklen_t addr_len = sizeof(sender_addr);
  char buffer[256];

  // Создаём UDP сокет
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    return 1;
  }

  // Настраиваем локальный адрес для приёма
  memset(&local_addr, 0, sizeof(local_addr));
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, BROADCAST_ADDR, &local_addr.sin_addr);

  // Привязываем сокет к порту
  if (bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
    perror("bind");
    return 1;
  }

  // Принимаем 10 сообщений
  int count = 0;
  while (count < 10) {
    memset(buffer, 0, sizeof(buffer));

    int bytes = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr *)&sender_addr, &addr_len);

    if (bytes < 0) {
      perror("recvfrom");
      continue;
    }

    count++;
    printf("[%d] Получено от %s:%d: %s\n", count,
           inet_ntoa(sender_addr.sin_addr), ntohs(sender_addr.sin_port),
           buffer);
  }

  printf("\nПолучено 10 сообщений\n");
  close(sock);
  return 0;
}