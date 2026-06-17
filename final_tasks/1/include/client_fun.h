#ifndef CLIENT_FUN_H
#define CLIENT_FUN_H

#include "common.h"

extern volatile int running;
extern int sock;
extern struct sockaddr_in server_addr;
extern uint16_t client_port;

void signal_handler(int sig);

#endif 
