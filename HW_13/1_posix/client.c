// client_posix.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>

#define QUEUE_NAME "/my_message_queue"
#define MAX_MSG_SIZE 1024

int main() {
    mqd_t mqdes;
    char buffer[MAX_MSG_SIZE];
    unsigned int priority;
    
    // Открытие существующей очереди (только чтение-запись)
    mqdes = mq_open(QUEUE_NAME, O_RDWR);
    if (mqdes == (mqd_t)-1) {
        perror("mq_open - очередь не найдена. Запустите сервер первым!");
        exit(1);
    }
    printf("Подключен к очереди, дескриптор: %d\n", mqdes);
    
    // Чтение сообщения от сервера
    ssize_t bytes = mq_receive(mqdes, buffer, MAX_MSG_SIZE, &priority);
    if (bytes == -1) {
        perror("mq_receive");
        mq_close(mqdes);
        exit(1);
    }
    printf("Клиент получил: %s (приоритет: %u)\n", buffer, priority);
    
    // Отправка ответа серверу
    strcpy(buffer, "Hello!");
    if (mq_send(mqdes, buffer, strlen(buffer) + 1, 2) == -1) {
        perror("mq_send");
        mq_close(mqdes);
        exit(1);
    }
    printf("Клиент отправил: %s\n", buffer);
    
    // Закрытие очереди
    if (mq_close(mqdes) == -1) {
        perror("mq_close");
        exit(1);
    }
    
    return 0;
}