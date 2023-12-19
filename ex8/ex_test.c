#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void child_handler(int sig)
{
    int status;
    pid_t pid;
    while ((pid = waitpid(0, &status, WNOHANG)) > 0)
    {
        //printf("Reaped one child [%d]\n", pid);
    }
}

int main(void)
{
    struct sigaction act = {.sa_handler = child_handler};
    sigfillset(&act.sa_mask);
    sigaction(SIGCHLD, &act, NULL);

    fork();
    fork();

    for (;;);

    return 0;
}