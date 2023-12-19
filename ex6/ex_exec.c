#include "stdio.h"
#include "unistd.h"
#include "sys/wait.h"

int main(int argc, char **argv)
{
    pid_t pid;
    if (argc < 2)
    {
        printf("Usage: %s PROG [PROG_ARG] ...\n", argv[0]);
        return 1;
    }

    pid = fork();
    argv[argc] = NULL;
    if (pid == 0)
    {
        execvp(argv[1], &argv[1]);
        perror("execv failed");
        return 1;
    }
    else if (pid < 0)
    {
        perror("fork failed");
        return 2;
    }

    wait(NULL);

    return 0;
}