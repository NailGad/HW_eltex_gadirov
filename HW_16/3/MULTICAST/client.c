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
  struct sockaddr_in local_addr, sender_addr;
  struct ip_mreqn mreq;
  socklen_t addr_len = sizeof(sender_addr);
  char buffer[256];

  // Создаём UDP сокет
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    return 1;
  }

  // Настраиваем локальный адрес
  memset(&local_addr, 0, sizeof(local_addr));
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(PORT);
  local_addr.sin_addr.s_addr = INADDR_ANY; // слушаем все интерфейсы

  // Привязываем сокет
  if (bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
    perror("bind");
    return 1;
  }

  // ПОДПИСЫВАЕМСЯ НА МУЛЬТИКАСТ-ГРУППУ
  memset(&mreq, 0, sizeof(mreq));
  inet_pton(AF_INET, MULTICAST_GROUP, &mreq.imr_multiaddr); // группа
  mreq.imr_address.s_addr = INADDR_ANY; // любой интерфейс
  mreq.imr_ifindex = 0; // индекс интерфейса (любой)

  if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) <
      0) {
    perror("setsockopt IP_ADD_MEMBERSHIP");
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

  // Отписываемся от группы 
  setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));

  printf("\nПолучено 10 сообщений\n");
  close(sock);
  return 0;
}