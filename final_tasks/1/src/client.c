#include "../include/client_fun.h"

int main() {
	struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        perror("sigaction");
        return 1;
    }
    
    srand(time(NULL));
    
    client_port = 30000 + (getpid() % 20000);
    
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock < 0) { perror("socket"); return 1; }
    
    int flag = 1;
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &flag, sizeof(flag));
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    
    printf("client (port %d)\n", client_port);
    printf("(put Ctrl+C or 'exit' to exit)\n");
    
    char input[256];
    char packet[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    
    while (running) {
        printf("> ");
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0) continue;

        if (strcmp(input, "exit") == 0) {
                // Отправляем "exit" серверу
                int len = build_udp_packet(packet, INADDR_ANY, inet_addr("127.0.0.1"),
                                            client_port, SERVER_PORT,
                                            "exit", strlen("exit") + 1);
                sendto(sock, packet, len, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
                
                printf("Exit\n");
                break;  
            }
        
        // Отправляем
        int len = build_udp_packet(packet, INADDR_ANY, inet_addr("127.0.0.1"),
                                    client_port, SERVER_PORT,
                                    input, strlen(input) + 1);
        sendto(sock, packet, len, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        
        // Ждём ответ именно на ЭТО сообщение
        struct timeval tv = {1, 0};
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        
        int found = 0;
        while (!found && running) {
            int bytes = recvfrom(sock, response, BUFFER_SIZE, 0, NULL, NULL);
            if (bytes < 0) {
                printf("no response\n");
                break;
            }
            
            if (bytes < 28) continue;
            
            struct udphdr *udp = (struct udphdr*)(response + 20);
            char *resp_data = response + 28;
            
            // Проверяем, что ответ от сервера и предназначен нам
            if (ntohs(udp->source) == SERVER_PORT && ntohs(udp->dest) == client_port) {
                printf("%s\n", resp_data);
                found = 1;
            }
        }
    }
    
    close(sock);
    return 0;
}