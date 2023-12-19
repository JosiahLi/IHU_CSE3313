#define _GNU_SOURCE

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <setjmp.h>

sigjmp_buf position;

void int_handler(int sig)
{
    //printf("\nSIGINT caught\n");
    static const char str[] = "\nSIGINIT caught\n";
    write(1, str, sizeof(str) - 1);
	siglongjmp(position, 1);
}

int main(void)
{
    struct sigaction act = {.sa_handler = int_handler};
	sigaction(SIGINT, &act, NULL);

    printf("int_handler() for SIGINT is registered\n");

	if (sigsetjmp(position, 0) != 0) printf("Restart sleeps...\n");

    for (int i = 0; i < 4; ++i)
    {
        printf("sleep call #%d\n", i);
        sleep(3);
    }

    return 0;
}
