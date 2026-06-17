#include "../include/server_fun.h"

volatile int running = 1;
client_node_t *clients = NULL;

void signal_handler(int sig, siginfo_t *info, void *context) {
    (void)info;
    (void)context;
    
    if (sig == SIGINT || sig == SIGTERM) {
        running = 0;
        printf("\nReceived signal %d shutting down\n", sig);
    }
}

client_node_t* find_client(uint32_t ip, uint16_t port) {
    client_node_t *cur = clients;
    while (cur) {
        if (cur->ip == ip && cur->port == port)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

client_node_t* add_client(uint32_t ip, uint16_t port) {
    client_node_t *new_node = malloc(sizeof(client_node_t));
    new_node->ip = ip;
    new_node->port = port;
    new_node->counter = 0;
    new_node->next = clients;
    clients = new_node;
    return new_node;
}

void remove_client(uint32_t ip, uint16_t port) {
    client_node_t *cur = clients, *prev = NULL;
    while (cur) {
        if (cur->ip == ip && cur->port == port) {
            if (prev)
                prev->next = cur->next;
            else
                clients = cur->next;
            free(cur);
            printf("Client %s:%d removed\n",
                   inet_ntoa(*(struct in_addr*)&ip), port);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void cleanup_all_clients(void) {
    client_node_t *cur = clients;
    int count = 0;
    while (cur) {
        client_node_t *next = cur->next;
        printf("del client %s:%d (had %d messages)\n",
               inet_ntoa(*(struct in_addr*)&cur->ip), cur->port, cur->counter);
        free(cur);
        cur = next;
        count++;
    }
    clients = NULL;
    if (count > 0)
        printf("Cleaned %d clients\n", count);
}
