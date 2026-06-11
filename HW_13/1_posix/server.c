// server_posix.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>

#define QUEUE_NAME "/my_message_queue"  // имя очереди (должно начинаться с /)
#define MAX_MSG_SIZE 1024
#define MAX_MSG_COUNT 10

int main() {
    mqd_t mqdes;
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];
    unsigned int priority;
    
    // 1. Настройка атрибутов очереди
    attr.mq_flags = 0;           // блокирующий режим
    attr.mq_maxmsg = MAX_MSG_COUNT;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    
    // 2. Создание очереди (права 0666)
    mqdes = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (mqdes == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }
    printf("Очередь создана, дескриптор: %d\n", mqdes);
    
    // 3. Отправка сообщения "Hi!" клиенту
    strcpy(buffer, "Hi!");
    if (mq_send(mqdes, buffer, strlen(buffer) + 1, 1) == -1) {
        perror("mq_send");
        mq_unlink(QUEUE_NAME);
        exit(1);
    }
    printf("Сервер отправил: %s\n", buffer);
    
    // 4. Ожидание ответа от клиента
    ssize_t bytes = mq_receive(mqdes, buffer, MAX_MSG_SIZE, &priority);
    if (bytes == -1) {
        perror("mq_receive");
        mq_unlink(QUEUE_NAME);
        exit(1);
    }
    printf("Сервер получил: %s (приоритет: %u)\n", buffer, priority);
    
    // 5. Закрытие очереди
    if (mq_close(mqdes) == -1) {
        perror("mq_close");
        exit(1);
    }
    
    // 6. Удаление очереди из системы
    if (mq_unlink(QUEUE_NAME) == -1) {
        perror("mq_unlink");
        exit(1);
    }
    printf("Очередь удалена\n");
    
    return 0;
}