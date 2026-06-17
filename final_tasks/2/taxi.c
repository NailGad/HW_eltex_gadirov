#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#define MAX_DRIVERS 20
#define INPUT_SIZE 32
#define SIZE_RESPONSE 128
#define NAME_LEN 32

/* ----- СТРУКТУРЫ ----- */
typedef enum {
    AVAILABLE,
    BUSY
} DriverStatus;

typedef struct {
    pid_t pid;
    int write_fd;   // менеджер → водитель
    int read_fd;    // водитель → менеджер
    DriverStatus status;
    char name[NAME_LEN];
} Driver;

Driver drivers[MAX_DRIVERS];
int drivers_count = 0;
int server_running = 1;
int pipe_fd[2];

/* ----- SIGACTION ----- */
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

/* ----- ВОДИТЕЛЬ ----- */
void run_driver(int read_fd, int write_fd) {
    DriverStatus status = AVAILABLE;
    time_t end_time = 0;
    char buffer[INPUT_SIZE];
    char name[NAME_LEN];

    // Ждём имя от менеджера
    memset(name, 0, NAME_LEN);
    int n = read(read_fd, name, NAME_LEN - 1);
    if (n <= 0) {
        // Если не получили имя, используем PID
        snprintf(name, NAME_LEN, "Driver_%d", getpid());
    }

    while (1) {
        fd_set read_fds;

        if (status == BUSY && time(NULL) >= end_time) {
            status = AVAILABLE;
            end_time = 0;
        }

        FD_ZERO(&read_fds);
        FD_SET(read_fd, &read_fds);

        struct timeval tv = {1, 0};
        int ret = select(read_fd + 1, &read_fds, NULL, NULL, &tv);
        if (ret == -1) {
            if (errno == EINTR) continue;
            break;
        }
        if (ret == 0) continue;  // таймаут

        if (FD_ISSET(read_fd, &read_fds)) {
            memset(buffer, 0, INPUT_SIZE);
            n = read(read_fd, buffer, INPUT_SIZE - 1);
            if (n <= 0) break;

            if (strncmp(buffer, "task", 4) == 0) {
                int seconds;
                sscanf(buffer, "task %d", &seconds);

                if (status == BUSY) {
                    int remaining = end_time - time(NULL);
                    if (remaining < 0) remaining = 0;
                    char response[SIZE_RESPONSE];
                    snprintf(response, sizeof(response), "%s (PID: %d): Busy %d sec", name, getpid(), remaining);
                    write(write_fd, response, strlen(response));
                } else {
                    status = BUSY;
                    end_time = time(NULL) + seconds;
                    char response[SIZE_RESPONSE];
                    snprintf(response, sizeof(response), "%s (PID: %d): Task received for %d sec", name, getpid(), seconds);
                    write(write_fd, response, strlen(response));
                }
            }
            else if (strcmp(buffer, "status") == 0) {
                char response[SIZE_RESPONSE];
                if (status == AVAILABLE) {
                    snprintf(response, sizeof(response), "%s (PID: %d): Available", name, getpid());
                } else {
                    int remaining = end_time - time(NULL);
                    if (remaining < 0) remaining = 0;
                    snprintf(response, sizeof(response), "%s (PID: %d): Busy %d sec", name, getpid(), remaining);
                }
                write(write_fd, response, strlen(response));
            }
        }
    }
}

// МЕНЕДЖЕР
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
        // Водитель
        close(to_driver[1]);
        close(from_driver[0]);

        run_driver(to_driver[0], from_driver[1]);
        exit(0);
    } else {
        // НЕ закрываем сразу, чтобы отправить имя
        close(to_driver[0]);
        close(from_driver[1]);

        drivers[drivers_count].pid = pid;
        drivers[drivers_count].write_fd = to_driver[1];
        drivers[drivers_count].read_fd = from_driver[0];
        drivers[drivers_count].status = AVAILABLE;
        strncpy(drivers[drivers_count].name, name, NAME_LEN - 1);

        // Отправляем имя водителю ЧЕРЕЗ pipe
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
        // Проверяем, что pipe открыт
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

/* ----- ТОЧКА ВХОДА ----- */
int main(void) {
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return 1;
    }

    setup_signals();

    printf("\n--- TAXI MANAGER (fork + pipe + select) ---\n");
    printf("Commands: create_driver <name>\n");
    printf("          send_task <pid> <timer>\n");
    printf("          get_status <pid>\n");
    printf("          get_drivers\n");
    printf("          exit\n\n");

    char input[INPUT_SIZE];
    char *args[10];
    int ac;

    while (server_running) {
        struct pollfd fds[2];
        fds[0].fd = STDIN_FILENO;
        fds[0].events = POLLIN;
        fds[1].fd = pipe_fd[0];
        fds[1].events = POLLIN;

        printf("> ");
        fflush(stdout);

        if (poll(fds, 2, -1) == -1) {
            if (errno == EINTR) continue;
            break;
        }

        if (fds[1].revents & POLLIN) {
            char dummy;
            read(pipe_fd[0], &dummy, 1);
            if (!server_running) break;
            continue;
        }

        if (!(fds[0].revents & POLLIN)) continue;

        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0) continue;

        ac = 0;
        char *tok = strtok(input, " ");
        while (tok && ac < 10) {
            args[ac++] = tok;
            tok = strtok(NULL, " ");
        }
        if (ac == 0) continue;

        if (strcmp(args[0], "create_driver") == 0) {
            if (ac < 2) { printf("Usage: create_driver <name>\n"); continue; }
            create_driver(args[1]);
        } else if (strcmp(args[0], "send_task") == 0) {
            if (ac < 3) { printf("Usage: send_task <pid> <timer>\n"); continue; }
            send_task(atoi(args[1]), atoi(args[2]));
        } else if (strcmp(args[0], "get_status") == 0) {
            if (ac < 2) { printf("Usage: get_status <pid>\n"); continue; }
            get_status(atoi(args[1]));
        } else if (strcmp(args[0], "get_drivers") == 0) {
            get_drivers();
        } else if (strcmp(args[0], "exit") == 0) {
            printf("Shutting down...\n");
            server_running = 0;
            write(pipe_fd[1], "x", 1);
        } else {
            printf("Unknown command: %s\n", args[0]);
        }
    }

    cleanup_all();
    return 0;
}