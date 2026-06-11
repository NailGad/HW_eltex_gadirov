#include "../include/client_fun.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <name>\n", argv[0]);
        return 1;
    }
    strncpy(my_name, argv[1], NAME_LEN - 1);

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }
    shared = mmap(NULL, sizeof(ChatData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        return 1;
    }
    close(shm_fd);

    sem_access = sem_open(SEM_ACCESS, 0);
    sem_server = sem_open(SEM_SERVER, 0);
    if (sem_access == SEM_FAILED || sem_server == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    // Ищем свободный слот
    sem_wait(sem_access);
    int free_slot = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!shared->clients[i].active && shared->clients[i].sem_name[0] == '\0') {
            free_slot = i;
            break;
        }
    }
    if (free_slot == -1) {
        sem_post(sem_access);
        printf("Server is full.\n");
        return 1;
    }
    my_client_id = free_slot;
    strncpy(shared->clients[my_client_id].name, my_name, NAME_LEN - 1);
    shared->clients[my_client_id].active = 0;
    sem_post(sem_access);

    // Создаём свой семафор
    char sem_name[64];
    snprintf(sem_name, 64, "/chat_client_%d", my_client_id);
    my_sem = sem_open(sem_name, O_CREAT, 0666, 0);
    if (my_sem == SEM_FAILED) {
        perror("sem_open client");
        return 1;
    }

    // Передаём имя семафора серверу
    sem_wait(sem_access);
    strncpy(shared->clients[my_client_id].sem_name, sem_name, 64);
    sem_post(sem_access);
    sem_post(sem_server);

    // Ждём подтверждения
    sem_wait(my_sem);

    sem_wait(sem_access);
    int accepted = shared->clients[my_client_id].active;
    sem_post(sem_access);

    if (!accepted) {
        printf("Name '%s' is already taken. Exiting.\n", my_name);
        sem_close(my_sem);
        sem_unlink(sem_name);
        munmap(shared, sizeof(ChatData));
        return 1;
    }

    init_ncurses();

    pthread_t recv_thread, send_thread;
    pthread_create(&recv_thread, NULL, receive_thread_func, NULL);
    pthread_detach(recv_thread);

    pthread_create(&send_thread, NULL, send_thread_func, NULL);
    pthread_join(send_thread, NULL);

    // Выход из клиента
    running = 0;
    sem_post(my_sem);
    pthread_join(recv_thread, NULL);

    sem_wait(sem_access);
    client_add_message("SYSTEM", "left", 1);
    shared->clients[my_client_id].active = 0;
    shared->clients[my_client_id].sem_name[0] = '\0';
    shared->client_count--;
    sem_post(sem_access);
    sem_post(sem_server);

    cleanup_ncurses();

    munmap(shared, sizeof(ChatData));
    sem_close(sem_access);
    sem_close(sem_server);
    sem_close(my_sem);
    sem_unlink(sem_name);

    return 0;
}