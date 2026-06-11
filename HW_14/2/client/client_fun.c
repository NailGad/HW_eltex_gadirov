#include "../include/client_fun.h"

ChatData *shared = NULL;
sem_t *sem_access = NULL;
sem_t *sem_server = NULL;
sem_t *my_sem = NULL;

WINDOW *chat_win = NULL;
WINDOW *users_win = NULL;
WINDOW *input_win = NULL;

char my_name[NAME_LEN] = "";
int running = 1;
int my_client_id = -1;

pthread_mutex_t ncurses_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_ncurses() {
    pthread_mutex_lock(&ncurses_mutex);
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, 1);
    curs_set(1);

    int h, w;
    getmaxyx(stdscr, h, w);
    int uw = w / 4, cw = w - uw, ch = h - 5;

    chat_win = newwin(ch, cw, 0, 0);
    scrollok(chat_win, 1);
    wrefresh(chat_win);

    users_win = newwin(ch, uw, 0, cw);
    box(users_win, 0, 0);
    mvwprintw(users_win, 0, 2, " Users ");
    wrefresh(users_win);

    input_win = newwin(3, w, ch, 0);
    box(input_win, 0, 0);
    mvwprintw(input_win, 0, 2, " Enter message (/q to quit): ");
    wrefresh(input_win);
    pthread_mutex_unlock(&ncurses_mutex);
}

void cleanup_ncurses() {
    pthread_mutex_lock(&ncurses_mutex);
    if (chat_win) delwin(chat_win);
    if (users_win) delwin(users_win);
    if (input_win) delwin(input_win);
    endwin();
    pthread_mutex_unlock(&ncurses_mutex);
}

void update_users_list() {
    pthread_mutex_lock(&ncurses_mutex);
    werase(users_win);
    box(users_win, 0, 0);
    mvwprintw(users_win, 0, 2, " Users ");

    int line = 1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (shared->clients[i].active) {
            if (i == my_client_id) {
                mvwprintw(users_win, line++, 2, "* %s (me)", shared->clients[i].name);
            } else {
                mvwprintw(users_win, line++, 2, "  %s", shared->clients[i].name);
            }
        }
    }
    wrefresh(users_win);
    pthread_mutex_unlock(&ncurses_mutex);
}

void update_chat() {
    pthread_mutex_lock(&ncurses_mutex);
    int h, w;
    getmaxyx(chat_win, h, w);
    werase(chat_win);

    int start = 0;
    if (shared->msg_count > h - 1) {
        start = shared->msg_count - (h - 1);
    }
    if (start < 0) start = 0;

    for (int i = start; i < shared->msg_count; i++) {
        char line[w + 1];
        memset(line, ' ', w);
        line[w] = '\0';

        if (shared->messages[i].is_system) {
            snprintf(line, w, ">>> %s", shared->messages[i].text);
        } else {
            snprintf(line, w, "[%s]: %s", shared->messages[i].sender, shared->messages[i].text);
        }
        mvwprintw(chat_win, i - start, 0, "%s", line);
    }
    wrefresh(chat_win);
    pthread_mutex_unlock(&ncurses_mutex);
}

void client_add_message(const char *sender, const char *text, int is_system) {
    if (shared->msg_count < MAX_MESSAGES) {
        int idx = shared->msg_count;
        strncpy(shared->messages[idx].sender, sender, NAME_LEN - 1);
        strncpy(shared->messages[idx].text, text, MSG_LEN - 1);
        shared->messages[idx].is_system = is_system;
        shared->msg_count++;
        shared->event_counter++;
    }
}

void *receive_thread_func(void *arg) {
    uint64_t last_event = 0;
    int last_client_count = 0;

    while (running) {
        sem_wait(my_sem);
        if (!running) break;

        sem_wait(sem_access);

        if (shared->event_counter != last_event) {
            update_chat();
            last_event = shared->event_counter;
        }

        if (shared->client_count != last_client_count) {
            update_users_list();
            last_client_count = shared->client_count;
        }

        sem_post(sem_access);
    }
    return NULL;
}

void *send_thread_func(void *arg) {
    char buffer[MSG_LEN];

    while (running) {
        pthread_mutex_lock(&ncurses_mutex);
        werase(input_win);
        box(input_win, 0, 0);
        mvwprintw(input_win, 0, 2, " Enter message (/q to quit): ");
        wrefresh(input_win);
        pthread_mutex_unlock(&ncurses_mutex);

        echo();
        curs_set(1);
        mvwgetnstr(input_win, 1, 2, buffer, MSG_LEN - 1);
        noecho();
        curs_set(0);

        if (strcmp(buffer, "/q") == 0) {
            running = 0;
            break;
        }

        if (strlen(buffer) > 0) {
            sem_wait(sem_access);
            client_add_message(my_name, buffer, 0);
            sem_post(sem_access);
            sem_post(sem_server);
        }
    }
    return NULL;
}