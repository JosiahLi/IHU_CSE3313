#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"

int main(void)
{
    pid_t pid;
    pid_t ppid;

    pid = getpid();
    ppid = getppid();

    printf("PID: %d, PPID: %d\n", pid, ppid);

    sleep(10);

    return 0;
}
