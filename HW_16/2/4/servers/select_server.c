#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 64

int main() {
  int tcp_fd, udp_fd, max_fd;
  struct sockaddr_in tcp_addr, udp_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);
  fd_set read_fds;
  char buffer[BUFFER_SIZE];
  time_t rawtime;

  // ==================== 1. СОЗДАЁМ TCP СОКЕТ ====================
  tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (tcp_fd < 0) {
    perror("TCP socket");
    return 1;
  }

  // ==================== 2. СОЗДАЁМ UDP СОКЕТ ====================
  udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp_fd < 0) {
    perror("UDP socket");
    return 1;
  }

  // НАСТРАИВАЕМ ПОРТ
  int opt = 1;
  setsockopt(tcp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  setsockopt(udp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // TCP адрес
  memset(&tcp_addr, 0, sizeof(tcp_addr));
  tcp_addr.sin_family = AF_INET;
  tcp_addr.sin_port = htons(PORT);
  tcp_addr.sin_addr.s_addr = INADDR_ANY;

  // UDP адрес (тот же порт!)
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
  // ОДИН ПОТОК обслуживает И TCP, И UDP!

  while (1) {
    // Обнуляем и заполняем множество сокетов
    FD_ZERO(&read_fds);
    FD_SET(tcp_fd, &read_fds); // следим за новыми TCP подключениями
    FD_SET(udp_fd, &read_fds); // следим за UDP сообщениями

    max_fd = (tcp_fd > udp_fd) ? tcp_fd : udp_fd;

    // Ждём события на ЛЮБОМ сокете
    int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

    if (activity < 0) {
      perror("select");
      continue;
    }

    // ОБРАБОТКА TCP КЛИЕНТОВ
    if (FD_ISSET(tcp_fd, &read_fds)) {
      int client_fd =
          accept(tcp_fd, (struct sockaddr *)&client_addr, &client_len);
      if (client_fd < 0) {
        perror("TCP accept");
        continue;
      }

      // Получаем текущее время
      time(&rawtime);
      ctime_r(&rawtime, buffer);
      buffer[strcspn(buffer, "\n")] = '\0';

      // Отправляем время TCP клиенту
      send(client_fd, buffer, strlen(buffer) + 1, 0);
      close(client_fd);

      printf("[TCP] Обслужен клиент %s:%d\n", inet_ntoa(client_addr.sin_addr),
             ntohs(client_addr.sin_port));
    }

    // ОБРАБОТКА UDP КЛИЕНТОВ
    if (FD_ISSET(udp_fd, &read_fds)) {
      memset(buffer, 0, BUFFER_SIZE);
      int n = recvfrom(udp_fd, buffer, BUFFER_SIZE - 1, 0,
                       (struct sockaddr *)&client_addr, &client_len);

      if (n < 0) {
        perror("UDP recvfrom");
        continue;
      }

      printf("[UDP] Получен запрос от %s:%d: '%s'\n",
             inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),
             buffer);

      // Получаем текущее время
      time(&rawtime);
      ctime_r(&rawtime, buffer);
      buffer[strcspn(buffer, "\n")] = '\0';

      // Отправляем время UDP клиенту
      sendto(udp_fd, buffer, strlen(buffer) + 1, 0,
             (struct sockaddr *)&client_addr, client_len);

      printf("[UDP] Отправлено время клиенту %s:%d\n",
             inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }
  }

  close(tcp_fd);
  close(udp_fd);

  return 0;
}