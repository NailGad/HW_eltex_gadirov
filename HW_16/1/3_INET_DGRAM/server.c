#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 32

int main() {
  // инициализация сокета
  int server_fd;
  struct sockaddr_in server_sock, client_sock;
  char buffer[BUFFER_SIZE];
  socklen_t size = sizeof(client_sock);

  server_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_fd < 0) {
    perror("socket");
    return 1;
  }
  // инициализация структуры адреса сервера
  memset(&server_sock, 0, sizeof(server_sock));
  server_sock.sin_family = AF_INET;
  server_sock.sin_port = htons(PORT);
  server_sock.sin_addr.s_addr = INADDR_ANY;
  // привязка сокета к адресу сервера
  if (bind(server_fd, (struct sockaddr *)&server_sock, sizeof(server_sock)) <
      0) {
    perror("bind");
    return 1;
  }

  // получение данных от клиента
  recvfrom(server_fd, buffer, sizeof(buffer), 0,
           (struct sockaddr *)&client_sock, &size);
  printf("%s\n", buffer);

  // отправка данных клиенту
  strcpy(buffer, "hi!");
  sendto(server_fd, buffer, strlen(buffer) + 1, 0,
         (struct sockaddr *)&client_sock, size);
  // закрытие сокета
  close(server_fd);
  return 0;
}