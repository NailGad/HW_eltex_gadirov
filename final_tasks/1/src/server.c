#include "../include/server_fun.h"


int main() {
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
    
    sa.sa_sigaction = signal_handler;  
    sa.sa_flags = SA_SIGINFO;           
    sigemptyset(&sa.sa_mask);     
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("sigaction");
        return 1;
    }
    
    srand(time(NULL));
    
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock < 0) { perror("socket"); return 1; }
    
    int flag = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &flag, sizeof(flag)) < 0) {
        perror("IP_HDRINCL"); return 1;
    }
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }
    
    printf("RAW UDP  port %d\n", SERVER_PORT);
    printf("(Ctrl+C-stop)\n");
    
    char buf[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    
    while (running) {
        struct sockaddr_in from;
        socklen_t from_len = sizeof(from);
        int bytes = recvfrom(sock, buf, BUFFER_SIZE, 0, (struct sockaddr*)&from, &from_len);
        if (bytes < 0 || bytes < 28) continue;
        
        struct udphdr *udp = (struct udphdr*)(buf + 20);
        char *data = buf + 28;
        
        if (ntohs(udp->dest) != SERVER_PORT) continue;
        
        uint32_t client_ip = from.sin_addr.s_addr;
        uint16_t client_port = ntohs(udp->source);
        
        // "exit" удаляем клиента
        if (strcmp(data, "exit") == 0) {
            remove_client(client_ip, client_port);
            continue;
        }
        
        client_node_t *client = find_client(client_ip, client_port);
        if (!client)
            client = add_client(client_ip, client_port);
        
        client->counter++;
        
        printf("%s:%d \"%s\" #%d\n",
               inet_ntoa(from.sin_addr), client_port, data, client->counter);
        
        char reply[256];
        int reply_len = snprintf(reply, sizeof(reply), "%s %d", data, client->counter);
        
        int packet_len = build_udp_packet(response, INADDR_ANY, client_ip,
                                          SERVER_PORT, client_port,
                                          reply, reply_len + 1);
        
        struct sockaddr_in dest;
        dest.sin_family = AF_INET;
        dest.sin_port = htons(client_port);
        dest.sin_addr.s_addr = client_ip;
        
        sendto(sock, response, packet_len, 0, (struct sockaddr*)&dest, sizeof(dest));
    }
    
    
    // Очищаем всех клиентов
    cleanup_all_clients();
    
    close(sock);
    printf("Shutdown\n");
    return 0;
}