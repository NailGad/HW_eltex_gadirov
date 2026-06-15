#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 256

int main() {
    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    // СОЗДАЁМ UDP СОКЕТ
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }
    
    // НАСТРАИВАЕМ АДРЕС СЕРВЕРА
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);      // сетевой порядок байт
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // ПРИВЯЗЫВАЕМ СОКЕТ
    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        return 1;
    }
    
    printf("Порт: %d\n", PORT);
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        
        // ПРИНИМАЕМ СООБЩЕНИЕ
        int bytes = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                            (struct sockaddr*)&client_addr, &client_len);
        
        if (bytes < 0) {
            perror("recvfrom");
            continue;
        }
        
        printf("Получено от %s:%d: %s\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
        
        // МОДИФИЦИРУЕМ ПРОИЗВОЛЬНЫЙ СИМВОЛ
        if (strlen(buffer) > 0) {
            // меняем первый символ
            buffer[0] = buffer[0] + 1;
            printf("Модифицировано: %s\n", buffer);
        }
        
        // ОТПРАВЛЯЕМ ОБРАТНО
        sendto(sock, buffer, strlen(buffer) + 1, 0,
               (struct sockaddr*)&client_addr, client_len);
        
        printf("Отправлено обратно клиенту %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }
    
    close(sock);
    return 0;
}