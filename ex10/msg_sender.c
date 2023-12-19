#include "msg_header.h"

int main(void)
{
    key_t key;
    int msgid;
    struct msg_entry msg;

    key = ftok("msg_header.h", 1);
    msgid = msgget(key, IPC_CREAT | QPERM);
    if (msgid == -1)
    {
        perror("msgget");
        return 1;
    }

    for (int i = 10; i > 0; i--)
    {
        msg.mtype = i;
        sprintf(msg.mtext, "message type %d", i);
        if (msgsnd(msgid, (void *)&msg, MAXLEN, IPC_NOWAIT) == -1)
        {
            perror("msgsnd");
            return 1;
        }
    }

    return 0;
}