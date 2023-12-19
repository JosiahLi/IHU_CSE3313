#define _GNU_SOURCE
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAXLEN 100
#define QPERM 0600
struct msg_entry 
{
    long mtype;
    char mtext[MAXLEN];
};