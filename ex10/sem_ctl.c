#include "sem_header.h"

int initsem(key_t semkey)
{
    union semun arg;
    int semid = semget(semkey, 2, 0600 | IPC_CREAT | IPC_EXCL);
    if (semid == -1)
    {
        perror("semget");
        exit(1);
    }

    unsigned short array[2] = {1, 2};
    arg.array = array;
    int status = semctl(semid, 0, SETALL, arg);
    if (status == -1)
    {
        perror("semctl - init");
        exit(1);
    }

    return semid;
}

void removesem(int semid)
{
    if (semctl(semid, 0, IPC_RMID, NULL) == -1)
    {
        perror("semctl - remove");
        exit(1);
    }
}

void p(int semid, int semidx)
{
    struct sembuf buf;

    buf.sem_num = semidx;
    buf.sem_op = -1;
    buf.sem_flg = SEM_UNDO | IPC_NOWAIT;

    while (semop(semid, &buf, 1) == -1)
    {
        printf("[sem: %d] Waiting Pid: %d\n", semidx, getpid());
        sleep(1);
    }
}

void v(int semid, int semidx)
{
    struct sembuf buf;

    buf.sem_num = semidx;
    buf.sem_op = 1;
    buf.sem_flg = SEM_UNDO;

    if (semop(semid, &buf, 1) == -1)
    {
        perror("semop -v");
        exit(1);
    }
}