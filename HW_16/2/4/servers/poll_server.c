#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 64
#define MAX_FDS 2

int main() {
  int tcp_fd, udp_fd, fd_count = 0;
  struct sockaddr_in tcp_addr, udp_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);
  struct pollfd fds[MAX_FDS];
  char buffer[BUFFER_SIZE];
  time_t rawtime;

  // СОЗДАЁМ TCP И UDP СОКЕТ
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

  // ПРИВЯЗЫВАЕМ СОКЕТЫ
  if (bind(tcp_fd, (struct sockaddr *)&tcp_addr, sizeof(tcp_addr)) < 0) {
    perror("TCP bind");
    return 1;
  }

  if (bind(udp_fd, (struct sockaddr *)&udp_addr, sizeof(udp_addr)) < 0) {
    perror("UDP bind");
    return 1;
  }

  // TCP: НАЧИНАЕМ СЛУШАТЬ
  if (listen(tcp_fd, 10) < 0) {
    perror("TCP listen");
    return 1;
  }

  // НАСТРАИВАЕМ POLL
  // Добавляем TCP сокет
  fds[fd_count].fd = tcp_fd;
  fds[fd_count].events = POLLIN; // интересуемся чтением
  fd_count++;

  // Добавляем UDP сокет
  fds[fd_count].fd = udp_fd;
  fds[fd_count].events = POLLIN;
  fd_count++;

  // ОСНОВНОЙ ЦИКЛ С POLL
  while (1) {
    // poll() ждёт события на любом из сокетов
    // параметры: массив fds, количество, таймаут (-1 = бесконечно)
    int ready = poll(fds, fd_count, -1);

    if (ready < 0) {
      perror("poll");
      continue;
    }

    // Проверяем каждый сокет
    for (int i = 0; i < fd_count; i++) {
      if (fds[i].revents & POLLIN) { // есть данные для чтения
        if (fds[i].fd == tcp_fd) {
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

          printf("[TCP] Обслужен клиент %s:%d\n",
                 inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        } else if (fds[i].fd == udp_fd) {
          // UDP КЛИЕНТ
          memset(buffer, 0, BUFFER_SIZE);
          recvfrom(udp_fd, buffer, BUFFER_SIZE - 1, 0,
                   (struct sockaddr *)&client_addr, &client_len);

          time(&rawtime);
          ctime_r(&rawtime, buffer);
          buffer[strcspn(buffer, "\n")] = '\0';

          sendto(udp_fd, buffer, strlen(buffer) + 1, 0,
                 (struct sockaddr *)&client_addr, client_len);

          printf("[UDP] Обслужен клиент %s:%d\n",
                 inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        }
      }
    }
  }

  close(tcp_fd);
  close(udp_fd);
  return 0;
}