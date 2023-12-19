#include "unistd.h"
#include "stdio.h"
#include "sys/types.h"

int main(int argc, char **argv)
{
    pid_t pid;
    printf("Calling fork..\n");

    pid = fork();

    if (!pid)
    {
        printf("I'm the child. pid: %d, getpid(): %d, getpid: %d\n", pid, getpid(), getppid());
        sleep(9);
    }    
    else if (pid > 0)
    {
        printf("I'm the parent. pid: %d, getpid(): %d, getpid: %d\n", pid, getpid(), getppid());
        sleep(10);
    }
    else printf("fork failed\n");

    return 0;
}