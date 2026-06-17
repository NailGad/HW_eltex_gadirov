#ifndef SERVER_FUN_H
#define SERVER_FUN_H

#include "common.h"

extern volatile int running;
extern client_node_t *clients;

void signal_handler(int sig, siginfo_t *info, void *context);
client_node_t* find_client(uint32_t ip, uint16_t port);
client_node_t* add_client(uint32_t ip, uint16_t port);
void remove_client(uint32_t ip, uint16_t port);
void cleanup_all_clients(void);

#endif 
