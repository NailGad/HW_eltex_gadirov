#include "../include/manager.h"
#include "../include/driver.h"

Driver drivers[MAX_DRIVERS];
int drivers_count = 0;
int server_running = 1;
int pipe_fd[2];

void sigaction_handler(int sig, siginfo_t *info, void *context) {
    (void)info; (void)context;
    if (sig == SIGINT || sig == SIGTERM) {
        server_running = 0;
        write(pipe_fd[1], "x", 1);
    }
}

void setup_signals(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = sigaction_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

int find_driver(pid_t pid) {
    for (int i = 0; i < drivers_count; i++) {
        if (drivers[i].pid == pid) return i;
    }
    return -1;
}

void create_driver(const char *name) {
    if (drivers_count >= MAX_DRIVERS) {
        printf("Maximum drivers reached (%d)\n", MAX_DRIVERS);
        return;
    }

    int to_driver[2];
    int from_driver[2];

    if (pipe(to_driver) == -1 || pipe(from_driver) == -1) {
        perror("pipe");
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        close(to_driver[0]); close(to_driver[1]);
        close(from_driver[0]); close(from_driver[1]);
        return;
    }

    if (pid == 0) {
        close(to_driver[1]);
        close(from_driver[0]);
        run_driver(to_driver[0], from_driver[1]);
        exit(0);
    } else {
        close(to_driver[0]);
        close(from_driver[1]);

        drivers[drivers_count].pid = pid;
        drivers[drivers_count].write_fd = to_driver[1];
        drivers[drivers_count].read_fd = from_driver[0];
        drivers[drivers_count].status = AVAILABLE;
        strncpy(drivers[drivers_count].name, name, NAME_LEN - 1);

        write(to_driver[1], name, strlen(name) + 1);

        drivers_count++;
        printf("Driver created: %s (PID: %d)\n", name, pid);
    }
}

void send_task(pid_t pid, int timer) {
    int id = find_driver(pid);
    if (id == -1) {
        printf("Driver with PID %d not found\n", pid);
        return;
    }

    char request[SIZE_RESPONSE];
    snprintf(request, sizeof(request), "task %d", timer);
    write(drivers[id].write_fd, request, strlen(request));

    char response[SIZE_RESPONSE];
    memset(response, 0, sizeof(response));
    int n = read(drivers[id].read_fd, response, sizeof(response) - 1);
    if (n > 0) {
        printf("%s\n", response);
    } else {
        printf("No response from driver %d\n", pid);
    }
}

void get_status(pid_t pid) {
    int id = find_driver(pid);
    if (id == -1) {
        printf("Driver with PID %d not found\n", pid);
        return;
    }

    write(drivers[id].write_fd, "status", 6);

    char response[SIZE_RESPONSE];
    memset(response, 0, sizeof(response));
    int n = read(drivers[id].read_fd, response, sizeof(response) - 1);
    if (n > 0) {
        printf("%s\n", response);
    } else {
        printf("No response from driver %d\n", pid);
    }
}

void get_drivers(void) {
    if (drivers_count == 0) {
        printf("No drivers\n");
        return;
    }

    printf("\n--- DRIVERS ---\n");
    for (int i = 0; i < drivers_count; i++) {
        if (drivers[i].write_fd == -1 || drivers[i].read_fd == -1) {
            printf("[%d] PID: %d | pipe closed\n", i, drivers[i].pid);
            continue;
        }

        write(drivers[i].write_fd, "status", 6);
        char response[SIZE_RESPONSE];
        memset(response, 0, sizeof(response));
        int n = read(drivers[i].read_fd, response, sizeof(response) - 1);
        if (n > 0) {
            printf("[%d] PID: %d | %s\n", i, drivers[i].pid, response);
        } else {
            printf("[%d] PID: %d | no response\n", i, drivers[i].pid);
        }
    }
    printf("Total: %d\n\n", drivers_count);
}

void cleanup_all(void) {
    for (int i = 0; i < drivers_count; i++) {
        if (drivers[i].pid > 0) {
            kill(drivers[i].pid, SIGTERM);
        }
        if (drivers[i].write_fd > 0) close(drivers[i].write_fd);
        if (drivers[i].read_fd > 0) close(drivers[i].read_fd);
    }
    usleep(100000);
    close(pipe_fd[0]);
    close(pipe_fd[1]);
}