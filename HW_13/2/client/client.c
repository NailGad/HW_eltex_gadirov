#include "client_fun.h"
int main() {
    struct mq_attr attr; //параметры очереди

    printf("Enter name: ");
    if (fgets(username, sizeof(username), stdin) == NULL) {
        exit(1);
    }
    username[strcspn(username, "\n")] = 0;
    if (strlen(username) == 0)
        exit(1);

    snprintf(client_qname, sizeof(client_qname), "/chat_client_%d", getpid());
    //инициализация атрибутов очереди
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    //открытие очереди для чтения
    client_q = mq_open(client_qname, O_CREAT | O_RDONLY, 0666, &attr);
    if (client_q == (mqd_t)-1) {
        perror("mq_open client");
        exit(1);
    }

    //открытие очереди для записи
    server_q = mq_open(SERVER_QUEUE, O_WRONLY);
    if (server_q == (mqd_t)-1) {
        perror("mq_open server");
        mq_unlink(client_qname);
        exit(1);
    }

    char reg[MAX_MSG_SIZE];
    snprintf(reg, sizeof(reg), "%s|%s", username, client_qname);
    send_msg("REGISTER", reg);

    init_ncurses();
    update_chat("___ WELCOME ___");
    update_chat("Type /q to exit\n");
    
    pthread_create(&recv_thread, NULL, receive_thread_func, NULL);
    pthread_detach(recv_thread); 
    
    pthread_create(&send_thread, NULL, send_thread_func, NULL);
    
    pthread_join(send_thread, NULL);

    send_msg("LEAVE", username);
    sleep(1);

    mq_close(client_q);
    mq_close(server_q);
    mq_unlink(client_qname);
    cleanup();

    return 0;
}