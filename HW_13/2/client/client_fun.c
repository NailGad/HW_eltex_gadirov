#include "client_fun.h"

// Глобальные переменные
mqd_t server_q, client_q;
char client_qname[100];
char username[MAX_NAME_SIZE];
pthread_t recv_thread, send_thread;

WINDOW *chat_win, *users_win, *input_win;
pthread_mutex_t win_mutex = PTHREAD_MUTEX_INITIALIZER;

//отправка сообщения в очередь клиента
void send_msg(const char *cmd, const char *data) {
    char buf[MAX_MSG_SIZE];
    if (data) {
        snprintf(buf, sizeof(buf), "%s|%s", cmd, data);
    } else {
        snprintf(buf, sizeof(buf), "%s|", cmd);
    }
    mq_send(server_q, buf, strlen(buf), 1);
}

//обновление окна чата
void update_chat(const char *msg) {
    pthread_mutex_lock(&win_mutex);
    wprintw(chat_win, "%s\n", msg);
    wrefresh(chat_win);
    pthread_mutex_unlock(&win_mutex);
}

//обновление окна пользователей
void update_users(const char *data) {
    pthread_mutex_lock(&win_mutex);
    werase(users_win);
    box(users_win, 0, 0);
    mvwprintw(users_win, 1, 2, "Users:");

    char copy[MAX_MSG_SIZE];
    strcpy(copy, data);
    char *name = strtok(copy, ",");
    int line = 3;
    while (name && line < getmaxy(users_win) - 1) {
        mvwprintw(users_win, line++, 2, "- %s", name);
        name = strtok(NULL, ",");
    }
    wrefresh(users_win);
    pthread_mutex_unlock(&win_mutex);
}
//поток получения сообщений от сервера
void *receive_thread_func(void *arg) {
    char buf[MAX_MSG_SIZE];
    unsigned int prio;

    while (1) {
        ssize_t bytes = mq_receive(client_q, buf, MAX_MSG_SIZE, &prio);
        if (bytes == -1) continue;
        
        buf[bytes] = '\0';
        
        char *type = strtok(buf, "|");
        char *data = strtok(NULL, "");
        if (!type) continue;

        if (strcmp(type, "SERVER") == 0)
            update_chat(data);
        else if (strcmp(type, "CHAT") == 0)
            update_chat(data);
        else if (strcmp(type, "USERS") == 0)
            update_users(data);
        else if (strcmp(type, "HISTORY") == 0)
            update_chat(data);
    }
    return NULL;
}
//поток отправки сообщений
void *send_thread_func(void *arg) {
    char input[MAX_MSG_SIZE - 50];
    char data[MAX_MSG_SIZE];
    int ch, pos = 0;
    memset(input, 0, sizeof(input));

    while (1) {
        pthread_mutex_lock(&win_mutex);
        werase(input_win);
        box(input_win, 0, 0);
        mvwprintw(input_win, 1, 2, "> %s", input);
        wrefresh(input_win);
        pthread_mutex_unlock(&win_mutex);

        ch = wgetch(input_win);

        if (ch == '\n') {
            if (strlen(input) > 0) {
                if (strcmp(input, "/q") == 0) {
                    return NULL;
                }
                snprintf(data, sizeof(data), "%s|%s", username, input);
                send_msg("MSG", data);
                memset(input, 0, sizeof(input));
                pos = 0;
            }
        } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if (pos > 0)
                input[--pos] = '\0';
        } else if (ch >= 32 && ch <= 126 && pos < MAX_MSG_SIZE - 1) {
            input[pos++] = ch;
            input[pos] = '\0';
        }
    }
    return NULL;
}
//инициализация ncurses
void init_ncurses(void) {
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
    wrefresh(users_win);
    
    input_win = newwin(3, w, ch, 0);
    box(input_win, 0, 0);
    wrefresh(input_win);
}
//очистка ресурсов ncurses
void cleanup(void) {
    delwin(chat_win);
    delwin(users_win);
    delwin(input_win);
    endwin();
}
