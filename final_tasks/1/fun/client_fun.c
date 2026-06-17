#include "../include/client_fun.h"


volatile int running = 1;
int sock;
struct sockaddr_in server_addr;
uint16_t client_port;

void signal_handler(int sig) {
    (void)sig;
    running = 0;
    
    char packet[BUFFER_SIZE];
    int len = build_udp_packet(packet, INADDR_ANY, inet_addr("127.0.0.1"),
                                client_port, SERVER_PORT,
                                "exit", strlen("exit") + 1);
    sendto(sock, packet, len, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("\nExit sent\n");
}
