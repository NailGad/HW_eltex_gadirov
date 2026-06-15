#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#define SERVER_PORT 8888
#define CLIENT_PORT 9999
#define BUFFER_SIZE 1024

// ДАННЫЕ КЛИЕНТА (ВАШ КОМПЬЮТЕР)
unsigned char src_mac[6] = {0xe0, 0xc2, 0x64, 0xbf, 0xac, 0xb6};  // MAC клиента
const char *client_ip = "192.168.0.31";        // IP клиента
const char *interface = "wlo1";                // интерфейс клиента

// ДАННЫЕ СЕРВЕРА (ДРУГОЙ КОМПЬЮТЕР)
unsigned char dst_mac[6] = {0xcc, 0x47, 0x40, 0x02, 0x80, 0x4a};  // MAC сервера
const char *server_ip = "192.168.0.20";        // IP сервера

uint16_t ip_checksum(uint16_t *data, int len) {
    uint32_t sum = 0;
    for (int i = 0; i < len / 2; i++) {
        sum += data[i];
    }
    if (len % 2 == 1) {
        sum += ((uint8_t*)data)[len - 1];
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return ~sum;
}

int main() {
    int sock;
    struct sockaddr_ll server_addr;
    char packet[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    struct ethhdr *eth;
    struct iphdr *ip;
    struct udphdr *udp;
    char *data;
    int data_len;
    int packet_len;
    struct ifreq ifr;
    int ifindex;
    
    // 1. СОЗДАЁМ RAW СОКЕТ
    sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock < 0) {
        perror("socket (нужны root права!)");
        return 1;
    }
    
    // 2. ПОЛУЧАЕМ ИНДЕКС ИНТЕРФЕЙСА (wlo1)
    strcpy(ifr.ifr_name, interface);
    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl SIOCGIFINDEX");
        close(sock);
        return 1;
    }
    ifindex = ifr.ifr_ifindex;
    

    printf("Интерфейс: %s (index %d)\n", interface, ifindex);
    printf("Source MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
           src_mac[0], src_mac[1], src_mac[2], src_mac[3], src_mac[4], src_mac[5]);
    printf("Source IP:  %s\n", client_ip);
    printf("Dest MAC:   %02X:%02X:%02X:%02X:%02X:%02X\n",
           dst_mac[0], dst_mac[1], dst_mac[2], dst_mac[3], dst_mac[4], dst_mac[5]);
    printf("Dest IP:    %s\n", server_ip);
    printf("UDP port:   %d (клиент) → %d (сервер)\n", CLIENT_PORT, SERVER_PORT);
    
    // 3. НАСТРАИВАЕМ АДРЕС ДЛЯ ОТПРАВКИ
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sll_family = AF_PACKET;
    server_addr.sll_protocol = htons(ETH_P_ALL);
    server_addr.sll_ifindex = ifindex;
    server_addr.sll_halen = 6;
    memcpy(server_addr.sll_addr, dst_mac, 6);
    
    // 4. ФОРМИРУЕМ ПАКЕТ
    memset(packet, 0, BUFFER_SIZE);
    
    // Ethernet заголовок
    eth = (struct ethhdr*)packet;
    memcpy(eth->h_dest, dst_mac, 6);
    memcpy(eth->h_source, src_mac, 6);
    eth->h_proto = htons(ETH_P_IP);
    
    // IP заголовок
    ip = (struct iphdr*)(packet + sizeof(struct ethhdr));
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->id = 0;
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = IPPROTO_UDP;
    ip->check = 0;
    ip->saddr = inet_addr(client_ip);
    ip->daddr = inet_addr(server_ip);
    
    // UDP заголовок
    udp = (struct udphdr*)(packet + sizeof(struct ethhdr) + sizeof(struct iphdr));
    udp->source = htons(CLIENT_PORT);
    udp->dest = htons(SERVER_PORT);
    data_len = strlen("hello!") + 1;
    udp->len = htons(sizeof(struct udphdr) + data_len);
    udp->check = 0;
    
    // Данные
    data = packet + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(data, "hello!");
    
    // Длина и контрольная сумма IP
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + data_len);
    ip->check = ip_checksum((uint16_t*)ip, sizeof(struct iphdr));
    
    packet_len = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr) + data_len;
    
    printf("\nПакет сформирован: %d байт\n", packet_len);
    printf("IP checksum: 0x%04X\n", ip->check);
    printf("Данные: '%s'\n", data);
    
    // ОТПРАВЛЯЕМ
    int bytes = sendto(sock, packet, packet_len, 0,
                       (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    if (bytes < 0) {
        perror("sendto");
        close(sock);
        return 1;
    }
    
    printf("\nОтправлено %d байт на сервер %s (%02X:%02X:%02X:%02X:%02X:%02X)\n", 
           bytes, server_ip,
           dst_mac[0], dst_mac[1], dst_mac[2], dst_mac[3], dst_mac[4], dst_mac[5]);
    
    while (1) {
        memset(response, 0, BUFFER_SIZE);
        
        bytes = recvfrom(sock, response, BUFFER_SIZE - 1, 0, NULL, NULL);
        if (bytes < 0) {
            perror("recvfrom");
            continue;
        }
        
        // Пропускаем слишком маленькие пакеты
        if (bytes < sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr)) {
            continue;
        }
        
        struct ethhdr *resp_eth = (struct ethhdr*)response;
        struct iphdr *resp_ip = (struct iphdr*)(response + sizeof(struct ethhdr));
        
        // Проверяем, что это IP пакет
        if (ntohs(resp_eth->h_proto) != ETH_P_IP) {
            continue;
        }
        
        int ip_header_len = resp_ip->ihl * 4;
        struct udphdr *resp_udp = (struct udphdr*)(response + sizeof(struct ethhdr) + ip_header_len);
        char *resp_data = response + sizeof(struct ethhdr) + ip_header_len + sizeof(struct udphdr);
        
        // Фильтруем: только ответ от сервера 
        if (ntohs(resp_udp->source) == SERVER_PORT) {
            printf("\nПолучен ответ от сервера:\n");
            printf("От MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                   resp_eth->h_source[0], resp_eth->h_source[1], resp_eth->h_source[2],
                   resp_eth->h_source[3], resp_eth->h_source[4], resp_eth->h_source[5]);
            printf("От IP: %s\n", inet_ntoa(*(struct in_addr*)&resp_ip->saddr));
            printf("UDP source port: %d (сервер)\n", ntohs(resp_udp->source));
            printf("Данные: '%s'\n", resp_data);
            printf("Модифицированная строка: %s\n", resp_data);
            break;
        }
    }
    
    close(sock);
    return 0;
}