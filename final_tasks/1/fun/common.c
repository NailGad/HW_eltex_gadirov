#include "../include/common.h"

// Подсчёт контрольной суммы IP
uint16_t ip_checksum(uint16_t *data, int len) {
    uint32_t sum = 0;
    for (int i = 0; i < (len + 1) / 2; i++)
        sum += data[i];
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}

void build_ip_header(struct iphdr *ip, uint32_t src, uint32_t dst, int total_len) {
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = htons(total_len);
    ip->id = htons(rand() % 65535);
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = IPPROTO_UDP;
    ip->check = 0;
    ip->saddr = src;
    ip->daddr = dst;
    ip->check = ip_checksum((uint16_t*)ip, sizeof(struct iphdr));
}

void build_udp_header(struct udphdr *udp, uint16_t src_port, uint16_t dst_port, int data_len) {
    udp->source = htons(src_port);
    udp->dest = htons(dst_port);
    udp->len = htons(sizeof(struct udphdr) + data_len);
    udp->check = 0;
}

int build_udp_packet(char *buf, uint32_t src_ip, uint32_t dst_ip,
                     uint16_t src_port, uint16_t dst_port,
                     const char *data, int data_len) {
    struct iphdr *ip = (struct iphdr*)buf;
    struct udphdr *udp = (struct udphdr*)(buf + sizeof(struct iphdr));
    char *payload = buf + sizeof(struct iphdr) + sizeof(struct udphdr);
    
    memcpy(payload, data, data_len);
    
    build_udp_header(udp, src_port, dst_port, data_len);
    build_ip_header(ip, src_ip, dst_ip, sizeof(struct iphdr) + sizeof(struct udphdr) + data_len);
    
    return sizeof(struct iphdr) + sizeof(struct udphdr) + data_len;
}