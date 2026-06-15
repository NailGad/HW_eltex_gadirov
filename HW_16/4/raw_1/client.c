#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#define SERVER_PORT 8888
#define CLIENT_PORT 9999
#define BUFFER_SIZE 256

int main() {
    int sock;
    struct sockaddr_in server_addr, from_addr;
    socklen_t from_len = sizeof(from_addr);
    char packet[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    struct udphdr *udp_header;
    char *data;
    int data_len;
    
    // СОЗДАЁМ RAW СОКЕТ
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock < 0) {
        perror("socket (нужны root права!)");
        return 1;
    }
    
    // НАСТРАИВАЕМ АДРЕС СЕРВЕРА
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    printf("Сервер: 127.0.0.1:%d\n", SERVER_PORT);
    printf("Клиентский порт: %d\n", CLIENT_PORT);
    
    // ФОРМИРУЕМ UDP ПАКЕТ
    memset(packet, 0, BUFFER_SIZE);
    
    udp_header = (struct udphdr*)packet;
    udp_header->source = htons(CLIENT_PORT);
    udp_header->dest = htons(SERVER_PORT);
    data_len = strlen("hello!") + 1;	
    udp_header->len = htons(sizeof(struct udphdr) + data_len);
    udp_header->check = 0;

    // смещаем для записи данных
    data = packet + sizeof(struct udphdr);
    strcpy(data, "hello!");
    
    printf("\nСформирован UDP пакет:\n");
    printf("   Source port: %d \n", CLIENT_PORT);
    printf("   Dest port:   %d \n", SERVER_PORT);
    printf("   UDP length:  %d байт\n", ntohs(udp_header->len));
    printf("   Data:        '%s'\n", data);
    
    // ОТПРАВЛЯЕМ
    int bytes = sendto(sock, packet, sizeof(struct udphdr) + data_len, 0,
                       (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    if (bytes < 0) {
        perror("sendto");
        close(sock);
        return 1;
    }
    
    printf("\nОтправлено %d байт на сервер\n", bytes);
    
    // ПОЛУЧАЕМ ОТВЕТ
    while (1) {
        memset(response, 0, BUFFER_SIZE);
        
        bytes = recvfrom(sock, response, BUFFER_SIZE - 1, 0,
                         (struct sockaddr*)&from_addr, &from_len);
        
        if (bytes < 0) {
            perror("recvfrom");
            continue;
        }
        udp_header = (struct udphdr*)(response + 20);       
        char *response_data = response + 20 + 8;
        
        // ФИЛЬТРУЕМ: пропускаем только пакеты ОТ СЕРВЕРА
        if (ntohs(udp_header->source) == SERVER_PORT) {
            printf("\nПолучен ответ от сервера:\n");
            printf("   UDP source port:  %d (порт сервера)\n", ntohs(udp_header->source));
            printf("   UDP dest port:    %d (наш порт)\n", ntohs(udp_header->dest));
            printf("   Данные: '%s'\n", response_data);
            printf("Модифицированная строка: %s\n", response_data);
            break;
        } else {
            printf("Пропущен свой пакет (source_port = %d)\n", ntohs(udp_header->source));
        }
    }
    
    close(sock);
    return 0;
}