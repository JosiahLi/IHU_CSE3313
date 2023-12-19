#include "sem_header.h"

int main(void)
{
    pid_t pid = getpid();
    int semid = initsem((key_t)pid);

    for (int i = 0; i < 4; ++i)
    {
        if (!fork())
        {
            pid = getpid();
            int semidx = i / 2;
            p(semid, semidx);
            printf("[sem: %d] Lock process %d[%d]\n", semidx, pid, i);
            sleep(1);
            printf("[sem: %d] Unlock process %d[%d]\n", semidx, pid, i);
            v(semid, semidx);
            sleep(1);
            printf("Exit process %d[%d]\n", pid, i);

            exit(0);
        }
    }

    for (int i = 0; i < 4; ++i) wait(NULL);

    removesem(semid);

    return 0;
}