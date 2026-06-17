#include "../include/driver.h"
#include "../include/common.h"

void run_driver(int read_fd, int write_fd) {
    DriverStatus status = AVAILABLE;
    time_t end_time = 0;
    char buffer[INPUT_SIZE];
    char name[NAME_LEN];

    memset(name, 0, NAME_LEN);
    int n = read(read_fd, name, NAME_LEN - 1);
    if (n <= 0) {
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
        if (ret == 0) continue;

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