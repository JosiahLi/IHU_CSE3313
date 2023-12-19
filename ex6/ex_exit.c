#include "stdio.h"
#include "unistd.h"
#include "sys/wait.h"
#include "stdlib.h"

int main(void)
{
    pid_t pid;
    int status, exit_status;
    if ((pid = fork()) < 0) return -1;

    if (pid == 0)
    {
        exit(1);
    }

    if ((pid = wait(&status)) == -1) // All of the children terminated
    {
        exit(2);
    }

    printf("status = %d\n", status);
    exit_status = WEXITSTATUS(status);
    printf("exit status = %d\n", exit_status);

    return 100;
}