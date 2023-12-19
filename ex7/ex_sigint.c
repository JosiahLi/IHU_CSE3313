#define _GNU_SOURCE

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void int_handler(int sig)
{
    //printf("\nSIGINT caught\n");
    static const char str[] = "\nSIGINIT caught\n";
    write(1, str, sizeof(str) - 1);
}

int main(void)
{
    struct sigaction act = {.sa_handler = int_handler};
    sigset_t set;
    //sigset_t oldset;
    //sigemptyset(&set);
    //sigaddset(&set, SIGINT);
    
    //sigfillset(&act.sa_mask);
    //act.sa_handler = int_handler;
    //act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &act, NULL);

    printf("int_handler() for SIGINT is registered\n");

    for (int i = 0; i < 4; ++i)
    {
        printf("sleep call #%d\n", i);
        //sigprocmask(SIG_BLOCK, &set, &oldset);
        sleep(3);
        //printf("sleep terminated %d\n", i);
        //sigprocmask(SIG_BLOCK, &oldset, NULL);
    }

    return 0;
}
