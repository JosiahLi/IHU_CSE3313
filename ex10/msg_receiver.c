#include "msg_header.h"

int main(void)
{
    key_t key;
    int msgid, msglen;
    struct msg_entry msg;

    key = ftok("msg_header.h", 1);
    msgid = msgget(key, IPC_CREAT | QPERM);
    if (msgid == -1)
    {
        perror("msgget");
        return 1;
    }

    while ((msglen = msgrcv(msgid, (void *)&msg, MAXLEN, -10, 0)) > 0)
    {
        printf("Received Message = %s\n", msg.mtext);
    } 

    return 0;
}