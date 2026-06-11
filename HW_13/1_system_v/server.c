// server_sysv.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MSG_FILE "/tmp"           // путь для ftok
#define PROJ_ID 123               // идентификатор проекта
#define MSG_TYPE_SERVER 1         // тип сообщения от сервера
#define MSG_TYPE_CLIENT 2         // тип сообщения от клиента
#define MSG_SIZE 1024             // размер буфера

struct msgbuf {
    long mtype;
    char mtext[MSG_SIZE];
};

int main() {
    key_t key;
    int msqid;
    struct msgbuf msg;
    
    // Генерация уникального ключа
    key = ftok(MSG_FILE, PROJ_ID);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }
    printf("Сгенерирован ключ: %d\n", key);
    
    // Создание очереди сообщений (права 0666)
    msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }
    printf("Очередь создана, ID: %d\n", msqid);
    
    // Отправка сообщения "Hi!" клиенту
    msg.mtype = MSG_TYPE_CLIENT;  // отправляем клиенту
    strcpy(msg.mtext, "Hi!");
    
    if (msgsnd(msqid, &msg, strlen(msg.mtext) + 1, 0) == -1) {
        perror("msgsnd (отправка Hi!)");
        msgctl(msqid, IPC_RMID, NULL);
        exit(1);
    }
    printf("Сервер отправил: %s\n", msg.mtext);
    
    //Ожидание ответа от клиента
    msg.mtype = MSG_TYPE_SERVER;  // ожидаем сообщение для сервера
    
    ssize_t bytes = msgrcv(msqid, &msg, MSG_SIZE, MSG_TYPE_SERVER, 0);
    if (bytes == -1) {
        perror("msgrcv (получение ответа)");
        msgctl(msqid, IPC_RMID, NULL);
        exit(1);
    }
    
    printf("Сервер получил: %s\n", msg.mtext);
    
    // 5. Удаление очереди
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl (удаление)");
        exit(1);
    }
    printf("Очередь удалена\n");
    
    return 0;
}