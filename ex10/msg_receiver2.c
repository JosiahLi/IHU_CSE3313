#include "msg_header.h"

int msgid;

void int_handler(int sig)
{
    if (msgctl(msgid, IPC_RMID, NULL) < 0)
    {
        perror("msgctl");
        exit(1);
    }
}

int main(void)
{
    key_t key;
    int msglen;
    struct msg_entry msg;

    struct sigaction action = {.sa_handler = int_handler};
    sigfillset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);

    key = ftok("msg_header.h", 1);
    msgid = msgget(key, IPC_CREAT | QPERM);
    if (msgid == -1)
    {
        perror("msgget");
        return 1;
    }

    while ((msglen = msgrcv(msgid, (void *)&msg, MAXLEN, -10, IPC_NOWAIT)) > 0)
    {
        printf("Received Message = %s\n", msg.mtext);
        sleep(1);
    } 

    if (msgctl(msgid, IPC_RMID, NULL) < 0)
    {
        perror("msgctl");
        return 1;
    }

    return 0;
}