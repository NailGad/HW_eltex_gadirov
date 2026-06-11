#include "server_fun.h"
Client clients[MAX_CLIENTS];
mqd_t server_queue;

//удаление истории
void delete_history() { remove(HISTORY_FILE); }

void save_to_history(const char *from, const char *msg) {
  FILE *f = fopen(HISTORY_FILE, "a");
  if (f) {
    fprintf(f, "%s: %s\n", from, msg);
    fclose(f);
  }
}
//отправка сообщения всем клиентам
void broadcast(const char *msg, int skip) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].active && i != skip) {
      mqd_t q = mq_open(clients[i].queue_name, O_WRONLY);
      if (q != (mqd_t)-1) {
        mq_send(q, msg, strlen(msg), 1);
        mq_close(q);
      }
    }
  }
}
//отправка списка пользователей
void send_user_list() {
  char list[MAX_MSG_SIZE] = "USERS|";
  int first = 1;
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].active) {
      if (!first)
        strcat(list, ",");
      strcat(list, clients[i].name);
      first = 0;
    }
  }
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].active) {
      mqd_t q = mq_open(clients[i].queue_name, O_WRONLY);
      if (q != (mqd_t)-1) {
        mq_send(q, list, strlen(list), 1);
        mq_close(q);
      }
    }
  }
}
//отправка истории
void send_history(const char *client_q) {
  FILE *f = fopen(HISTORY_FILE, "r");
  mqd_t q = mq_open(client_q, O_WRONLY);
  if (q == (mqd_t)-1)
    return;

  if (!f) {
    mq_send(q, "HISTORY|Empty", strlen("HISTORY|Empty"), 1);
    mq_close(q);
    return;
  }

  char line[MAX_MSG_SIZE-11];
  while (fgets(line, sizeof(line), f)) {
    line[strcspn(line, "\n")] = 0;
    char msg[MAX_MSG_SIZE];
    snprintf(msg, sizeof(msg), "HISTORY|%s", line);
    mq_send(q, msg, strlen(msg), 1);
  }
  fclose(f);
  mq_close(q);
}
//находим клиента по имени
int find_client(const char *name) {
  for (int i = 0; i < MAX_CLIENTS; i++)
    if (clients[i].active && strcmp(clients[i].name, name) == 0)
      return i;
  return -1;
}
//добавляем клиента
void add_client(const char *name, const char *qname) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (!clients[i].active) {
      strcpy(clients[i].name, name);
      strcpy(clients[i].queue_name, qname);
      clients[i].active = 1;

      send_history(qname);

      char msg[MAX_MSG_SIZE];
      snprintf(msg, sizeof(msg), "SERVER|%s joined", name);
      broadcast(msg, -1);
      send_user_list();

      printf("%s joined\n", name);
      return;
    }
  }
}
//удаляем клиента
void remove_client(const char *name) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].active && strcmp(clients[i].name, name) == 0) {
      clients[i].active = 0;

      char msg[MAX_MSG_SIZE];
      snprintf(msg, sizeof(msg), "SERVER|%s left", name);
      broadcast(msg, -1);
      send_user_list();

      printf("%s left\n", name);
      return;
    }
  }
}
//сигнал
void signal_handler(int sig) {
    delete_history();
    mq_close(server_queue);
    mq_unlink(SERVER_QUEUE);
    exit(0);
}