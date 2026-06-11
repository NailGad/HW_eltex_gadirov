#include "server_fun.h"
int main() {
  struct mq_attr attr; //задаем параметры очереди
  char buf[MAX_MSG_SIZE];
  unsigned int prio;

  delete_history();
  memset(clients, 0, sizeof(clients));
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
//открываем очередь
  server_queue = mq_open(SERVER_QUEUE, O_CREAT | O_RDONLY, 0666, &attr);
  if (server_queue == (mqd_t)-1) {
    perror("mq_open");
    exit(1);
  }//инициализируем сигнал
  signal(SIGINT, signal_handler);

  printf("|___CHAT___SERVER___|\n");
 
  while (1) {
    ssize_t bytes = mq_receive(server_queue, buf, MAX_MSG_SIZE, &prio);
    if (bytes == -1)
      break;

    buf[bytes] = '\0';

    char *cmd = strtok(buf, "|");
    char *data = strtok(NULL, "");

    if (!cmd)
      continue;

    if (strcmp(cmd, "REGISTER") == 0 && data) {
      char *name = strtok(data, "|");
      char *qname = strtok(NULL, "");
      if (name && qname) {
        if (find_client(name) != -1) {
          mqd_t q = mq_open(qname, O_WRONLY);
          if (q != (mqd_t)-1) {
            mq_send(q, "SERVER|Name taken", strlen("SERVER|Name taken"), 1);
            mq_close(q);
          }
        } else {
          add_client(name, qname);
        }
      }
    } else if (strcmp(cmd, "MSG") == 0 && data) {
      char *name = strtok(data, "|");
      char *text = strtok(NULL, "");
      if (name && text) {
        char msg[MAX_MSG_SIZE];
        snprintf(msg, sizeof(msg), "CHAT|%s: %s", name, text);
        broadcast(msg, -1);
        save_to_history(name, text);
      }
    } else if (strcmp(cmd, "LEAVE") == 0 && data) {
      remove_client(data);
    }
  }
  return 0;
}