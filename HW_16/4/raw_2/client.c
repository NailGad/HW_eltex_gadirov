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
    struct iphdr *ip_header;
    struct udphdr *udp_header;
    char *data;
    int data_len;
    int packet_len;
    
    // СОЗДАЁМ RAW СОКЕТ
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock < 0) {
        perror("socket (нужны root права!)");
        return 1;
    }
    
    // ВКЛЮЧАЕМ IP_HDRINCL (разрешаем сами формировать IP заголовок)
    int flag = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &flag, sizeof(flag)) < 0) {
        perror("setsockopt IP_HDRINCL");
        close(sock);
        return 1;
    }
    
    // НАСТРАИВАЕМ АДРЕС (нужен для маршрутизации)
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    
    printf("Сервер: 127.0.0.1:%d\n", SERVER_PORT);
    printf("лиентский порт: %d\n", CLIENT_PORT);

    
    // ФОРМИРУЕМ ПОЛНЫЙ IP и UDP ПАКЕТ
    memset(packet, 0, BUFFER_SIZE);
    
    // IP ЗАГОЛОВОК (20 байт)
    ip_header = (struct iphdr*)packet;
    ip_header->ihl = 5;                    // длина IP заголовка = 5 * 4 = 20 байт
    ip_header->version = 4;                // IPv4
    ip_header->protocol = IPPROTO_UDP;     // протокол UDP
    ip_header->check = 0;                  // сначала 0, потом вычислим
    ip_header->saddr = inet_addr("127.0.0.1");     // наш IP (можно 0, ядро подставит)
    ip_header->daddr = inet_addr("127.0.0.1");     // IP сервера
    
    // UDP ЗАГОЛОВОК (8 байт)
    udp_header = (struct udphdr*)(packet + sizeof(struct iphdr));
    udp_header->source = htons(CLIENT_PORT);
    udp_header->dest = htons(SERVER_PORT);
    data_len = strlen("hello!") + 1;
    udp_header->len = htons(sizeof(struct udphdr) + data_len);
    udp_header->check = 0;                 // checksum = 0 
    
    // ДАННЫЕ
    data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(data, "hello!");
    
    // ВЫЧИСЛЯЕМ ОБЩУЮ ДЛИНУ ПАКЕТА
    packet_len = sizeof(struct iphdr) + sizeof(struct udphdr) + data_len;
    ip_header->tot_len = htons(packet_len);
    
 
    ip_header->check = 0;
    
    // Выводим информацию
    printf("\nСформирован полный IP+UDP пакет:\n");
    printf("   IP header:\n");
    printf("     Version/IHL: %d/%d\n", ip_header->version, ip_header->ihl);
    printf("     Total len:   %d байт\n", ntohs(ip_header->tot_len));
    printf("     Protocol:    %d (UDP)\n", ip_header->protocol);
    printf("     Src IP:      %s\n", inet_ntoa(*(struct in_addr*)&ip_header->saddr));
    printf("     Dst IP:      %s\n", inet_ntoa(*(struct in_addr*)&ip_header->daddr));
    printf("   UDP header:\n");
    printf("     Source port: %d\n", ntohs(udp_header->source));
    printf("     Dest port:   %d\n", ntohs(udp_header->dest));
    printf("     UDP length:  %d байт\n", ntohs(udp_header->len));
    printf("   Data: '%s'\n", data);
    
    // ОТПРАВЛЯЕМ 
    int bytes = sendto(sock, packet, packet_len, 0,
                       (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    if (bytes < 0) {
        perror("sendto");
        close(sock);
        return 1;
    }
    
    printf("\nОтправлено %d байт на сервер (полный IP+UDP пакет)\n", bytes);
    
    // ПОЛУЧАЕМ ОТВЕТ 
    while (1) {
        memset(response, 0, BUFFER_SIZE);
        
        bytes = recvfrom(sock, response, BUFFER_SIZE - 1, 0,
                         (struct sockaddr*)&from_addr, &from_len);
        
        if (bytes < 0) {
            perror("recvfrom");
            continue;
        }
        
        // смещение
        udp_header = (struct udphdr*)(response + 20);
        char *response_data = response + 20 + 8;
        
        // Фильтруем: только пакеты ОТ СЕРВЕРА
        if (ntohs(udp_header->source) == SERVER_PORT) {
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