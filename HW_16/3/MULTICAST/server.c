#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 7777
#define MULTICAST_GROUP "224.0.0.1"

int main() {
  int sock;
  struct sockaddr_in group_addr;
  char buffer[256];

  // Создаём UDP сокет
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    return 1;
  }

  // Настраиваем адрес мультикаст-группы
  memset(&group_addr, 0, sizeof(group_addr));
  group_addr.sin_family = AF_INET;
  group_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, MULTICAST_GROUP, &group_addr.sin_addr);


  // Отправляем 10 сообщений в группу
  for (int i = 1; i <= 10; i++) {
    snprintf(buffer, sizeof(buffer), "Multicast сообщение #%d от сервера", i);

    int bytes = sendto(sock, buffer, strlen(buffer) + 1, 0,
                       (struct sockaddr *)&group_addr, sizeof(group_addr));

    if (bytes < 0) {
      perror("sendto");
    } else {
      printf("[%d] Отправлено в группу %s: %s\n", i, MULTICAST_GROUP, buffer);
    }

    sleep(1);
  }

  printf("\nОтправка завершена\n");
  close(sock);
  return 0;
}