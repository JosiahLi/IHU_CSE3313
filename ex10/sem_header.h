#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
union semun 
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
int initsem(key_t semkey);
void removesem(int semid);
void p(int semid, int semidx);
void v(int semid, int semidx);