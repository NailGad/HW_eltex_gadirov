#include "../include/common.h"
#include "../include/manager.h"

int main(void) {
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return 1;
    }

    setup_signals();

    printf("\nTAXI MANAGER (fork + pipe + poll + select + sigaction)\n");
    printf("Commands:\ncreate_driver <name>\nsend_task <pid> <timer>\nget_status <pid>\nget_drivers\nexit\n\n");

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