#define _GNU_SOURCE

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void fpe_handler(int sig)
{
    printf("\nSIGFPE caught\n");

    exit(1);
}

int main(void)
{
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_handler = fpe_handler;
    sigaction(SIGFPE, &act, NULL);

    int error = 10 / 0;

    return 0;
}