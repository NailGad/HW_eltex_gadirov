#include "../include/server_fun.h"

int main() {
    signal(SIGINT, cleanup);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(ChatData));
    shared = mmap(NULL, sizeof(ChatData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    close(shm_fd);
    memset(shared, 0, sizeof(ChatData));
    shared->server_running = 1;

    sem_access = sem_open(SEM_ACCESS, O_CREAT, 0666, 1);
    sem_server = sem_open(SEM_SERVER, O_CREAT, 0666, 0);

    printf("Server started. PID: %d\n", getpid());

    uint64_t last_event = 0;

    while (shared->server_running) {
        sem_wait(sem_server);
        sem_wait(sem_access);

        int changed = 0;

        //  Обработка новых клиентов
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (shared->clients[i].sem_name[0] != '\0' && shared->clients[i].active == 0) {
                if (is_name_taken(shared->clients[i].name)) {
                    // отклоняем
                    sem_t *sem = sem_open(shared->clients[i].sem_name, 0);
                    if (sem != SEM_FAILED) {
                        sem_post(sem);
                        sem_close(sem);
                    }
                    shared->clients[i].sem_name[0] = '\0';
                    shared->clients[i].name[0] = '\0';
                } else {
                    shared->clients[i].active = 1;
                    char msg[MSG_LEN];
                    snprintf(msg, MSG_LEN, "%s joined", shared->clients[i].name);
                    add_message("SYSTEM", msg, 1);
                    printf("%s joined\n", shared->clients[i].name);
                    // уведомляем нового клиента
                    sem_t *sem = sem_open(shared->clients[i].sem_name, 0);
                    if (sem != SEM_FAILED) {
                        sem_post(sem);
                        sem_close(sem);
                    }
                    changed = 1;
                }
            }
        }

        //  Обработка новых сообщений
        if (shared->event_counter != last_event) {
            changed = 1;
            last_event = shared->event_counter;
        }

        if (changed) {
            notify_all_clients();
        }

        sem_post(sem_access);
    }

    cleanup(0);
    return 0;
}