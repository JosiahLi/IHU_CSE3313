#include "stdio.h"
#include "unistd.h"
#include "sys/wait.h"
#include "stdlib.h"

int main(int argc, char **argv)
{
    pid_t pid;
    if (argc != 3)
    {
        printf("Uage: %s <parent sleep time> <child sleep time>\n", argv[0]);
        return 1;
    }

    int p_time = atoi(argv[1]), c_time = atoi(argv[2]);

    pid = fork();
    if (pid == 0)
    {
        sleep(c_time);
        printf("Child process [%d] is done. Now child becomes zombie.\n", getpid());
    }
    else if (pid > 0)
    {
        sleep(p_time);
        printf("Parent precess [%d] is done. Now chlid becomes orphan if it is alive.\n", getpid());
    }

    return 0;
}