// client_sysv.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MSG_FILE "/tmp"
#define PROJ_ID 123
#define MSG_TYPE_SERVER 1
#define MSG_TYPE_CLIENT 2
#define MSG_SIZE 1024

struct msgbuf {
    long mtype;
    char mtext[MSG_SIZE];
};

int main() {
    key_t key;
    int msqid;
    struct msgbuf msg;
    
    // Получение ключа (должен совпадать с сервером)
    key = ftok(MSG_FILE, PROJ_ID);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }
    
    // Подключение к существующей очереди
    msqid = msgget(key, 0666);
    if (msqid == -1) {
        perror("msgget - очередь не найдена. Запустите сервер первым!");
        exit(1);
    }
    printf("Подключен к очереди, ID: %d\n", msqid);
    
    // Чтение сообщения от сервера 
    ssize_t bytes = msgrcv(msqid, &msg, MSG_SIZE, MSG_TYPE_CLIENT, 0);
    if (bytes == -1) {
        perror("msgrcv");
        exit(1);
    }
    printf("Клиент получил: %s\n", msg.mtext);
    
    // Отправка ответа серверу
    msg.mtype = MSG_TYPE_SERVER;
    strcpy(msg.mtext, "Hello!");
    
    if (msgsnd(msqid, &msg, strlen(msg.mtext) + 1, 0) == -1) {
        perror("msgsnd");
        exit(1);
    }
    printf("Клиент отправил: %s\n", msg.mtext);
    
    return 0;
}