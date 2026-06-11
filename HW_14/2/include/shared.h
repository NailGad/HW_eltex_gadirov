#ifndef SHARED_H
#define SHARED_H

#include <stdint.h>

#define MAX_CLIENTS 10
#define MAX_MESSAGES 1000  
#define MSG_LEN 256
#define NAME_LEN 32

typedef struct {
    char name[NAME_LEN];
    int active;
    char sem_name[64];
} Client;

typedef struct {
    char sender[NAME_LEN];
    char text[MSG_LEN];
    int is_system;
} Message;

typedef struct {
    Client clients[MAX_CLIENTS];
    int client_count;

    Message messages[MAX_MESSAGES];
    int msg_count;                   

    uint64_t event_counter;         

    int server_running;
} ChatData;

#endif