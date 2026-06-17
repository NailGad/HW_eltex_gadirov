#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>

#define SERVER_PORT 8888
#define BUFFER_SIZE 256

// Структура клиента
typedef struct client_node {
    uint32_t ip;
    uint16_t port;
    int counter;
    struct client_node *next;
} client_node_t;

// Прототипы
uint16_t ip_checksum(uint16_t *data, int len);
void build_ip_header(struct iphdr *ip, uint32_t src, uint32_t dst, int total_len);
void build_udp_header(struct udphdr *udp, uint16_t src_port, uint16_t dst_port, int data_len);
int build_udp_packet(char *buf, uint32_t src_ip, uint32_t dst_ip,
                     uint16_t src_port, uint16_t dst_port,
                     const char *data, int data_len);

#endif