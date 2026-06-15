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
  struct sockaddr_in broadcast_addr;
  int flag = 1;
  char buffer[256];

  // Создаём UDP сокет
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    return 1;
  }

  // РАЗРЕШАЕМ BROADCAST (ключевой момент!)
  if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag)) < 0) {
    perror("setsockopt SO_BROADCAST");
    return 1;
  }

  // Настраиваем адрес для broadcast
  memset(&broadcast_addr, 0, sizeof(broadcast_addr));
  broadcast_addr.sin_family = AF_INET;
  broadcast_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, BROADCAST_ADDR, &broadcast_addr.sin_addr);

  // Отправляем 10 сообщений
  for (int i = 1; i <= 10; i++) {
    snprintf(buffer, sizeof(buffer), "Broadcast сообщение #%d от сервера", i);

    int bytes =
        sendto(sock, buffer, strlen(buffer) + 1, 0,
               (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));

    if (bytes < 0) {
      perror("sendto");
    } else {
      printf("[%d] Отправлено: %s\n", i, buffer);
    }

    sleep(1); // пауза между отправками
  }

  printf("\nОтправка завершена\n");
  close(sock);
  return 0;
}