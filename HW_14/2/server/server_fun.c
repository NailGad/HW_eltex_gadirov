#include "../include/server_fun.h"

ChatData *shared = NULL;
sem_t *sem_access = NULL;
sem_t *sem_server = NULL;

void cleanup(int sig) {
    (void)sig;
    if (shared) {
        shared->server_running = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (shared->clients[i].active) {
                sem_t *sem = sem_open(shared->clients[i].sem_name, 0);
                if (sem != SEM_FAILED) {
                    sem_post(sem);
                    sem_close(sem);
                }
            }
        }
        munmap(shared, sizeof(ChatData));
    }
    if (sem_access) sem_close(sem_access);
    if (sem_server) sem_close(sem_server);
    sem_unlink(SEM_ACCESS);
    sem_unlink(SEM_SERVER);
    shm_unlink(SHM_NAME);
    printf("\nServer stopped.\n");
    exit(0);
}

void add_message(const char *sender, const char *text, int is_system) {
    if (shared->msg_count < MAX_MESSAGES) {
        int idx = shared->msg_count;
        strncpy(shared->messages[idx].sender, sender, NAME_LEN - 1);
        strncpy(shared->messages[idx].text, text, MSG_LEN - 1);
        shared->messages[idx].is_system = is_system;
        shared->msg_count++;
        shared->event_counter++;
    }
}

void notify_all_clients() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (shared->clients[i].active) {
            sem_t *sem = sem_open(shared->clients[i].sem_name, 0);
            if (sem != SEM_FAILED) {
                sem_post(sem);
                sem_close(sem);
            }
        }
    }
}

int is_name_taken(const char *name) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (shared->clients[i].active && strcmp(shared->clients[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}