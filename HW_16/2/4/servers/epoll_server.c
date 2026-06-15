#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 64
#define MAX_EVENTS 10

int main() {
  int tcp_fd, udp_fd, epfd;
  struct sockaddr_in tcp_addr, udp_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);
  struct epoll_event ev, events[MAX_EVENTS];
  char buffer[BUFFER_SIZE];
  time_t rawtime;

  // СОЗДАЁМ СОКЕТЫ
  tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
  udp_fd = socket(AF_INET, SOCK_DGRAM, 0);

  if (tcp_fd < 0 || udp_fd < 0) {
    perror("socket");
    return 1;
  }

  // НАСТРАИВАЕМ АДРЕСА
  int opt = 1;
  setsockopt(tcp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  setsockopt(udp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  memset(&tcp_addr, 0, sizeof(tcp_addr));
  tcp_addr.sin_family = AF_INET;
  tcp_addr.sin_port = htons(PORT);
  tcp_addr.sin_addr.s_addr = INADDR_ANY;

  memset(&udp_addr, 0, sizeof(udp_addr));
  udp_addr.sin_family = AF_INET;
  udp_addr.sin_port = htons(PORT);
  udp_addr.sin_addr.s_addr = INADDR_ANY;

  // ПРИВЯЗЫВАЕМ
  if (bind(tcp_fd, (struct sockaddr *)&tcp_addr, sizeof(tcp_addr)) < 0) {
    perror("bind: tcp_fd");
    return 1;
  }
  if (bind(udp_fd, (struct sockaddr *)&udp_addr, sizeof(udp_addr)) < 0) {
    perror("bind: udp_fd");
    return 1;
  }
  if (listen(tcp_fd, 10) < 0) {
    perror("listen: tcp_fd");
    return 1;
  }

  // СОЗДАЁМ EPOLL ИНСТАНС
  epfd = epoll_create1(0);
  if (epfd < 0) {
    perror("epoll_create1");
    return 1;
  }

  // 5. ДОБАВЛЯЕМ TCP СОКЕТ
  ev.events = EPOLLIN; // интересуемся чтением
  ev.data.fd = tcp_fd; // сохраняем fd с данными
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, tcp_fd, &ev) < 0) {
    perror("epoll_ctl: tcp_fd");
    return 1;
  }

  // 6. ДОБАВЛЯЕМ UDP СОКЕТ
  ev.events = EPOLLIN;
  ev.data.fd = udp_fd;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, udp_fd, &ev) < 0) {
    perror("epoll_ctl: udp_fd");
    return 1;
  }

  // 7. ОСНОВНОЙ ЦИКЛ С EPOLL
  while (1) {
    // epoll_wait() возвращает ТОЛЬКО готовые дескрипторы
    // O(1) — не сканирует все fd!
    int n = epoll_wait(epfd, events, MAX_EVENTS, -1);

    if (n < 0) {
      perror("epoll_wait");
      continue;
    }

    // Обрабатываем ТОЛЬКО те сокеты, которые готовы
    for (int i = 0; i < n; i++) {
      if (events[i].data.fd == tcp_fd) {
        // TCP КЛИЕНТ
        int client_fd =
            accept(tcp_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
          continue;

        time(&rawtime);
        ctime_r(&rawtime, buffer);
        buffer[strcspn(buffer, "\n")] = '\0';

        send(client_fd, buffer, strlen(buffer) + 1, 0);
        close(client_fd);

        printf("[TCP] Обслужен клиент %s:%d\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));
      } else if (events[i].data.fd == udp_fd) {
        // UDP КЛИЕНТ
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(udp_fd, buffer, BUFFER_SIZE - 1, 0,
                 (struct sockaddr *)&client_addr, &client_len);

        time(&rawtime);
        ctime_r(&rawtime, buffer);
        buffer[strcspn(buffer, "\n")] = '\0';

        sendto(udp_fd, buffer, strlen(buffer) + 1, 0,
               (struct sockaddr *)&client_addr, client_len);

        printf("[UDP] Обслужен клиент %s:%d\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));
      }
    }
  }

  close(tcp_fd);
  close(udp_fd);
  close(epfd);
  return 0;
}