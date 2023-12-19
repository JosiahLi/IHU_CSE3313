#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>

int main(int argc, char **argv)
{
    int opt, flag_g = 0, pid, signum;

    while ((opt = getopt(argc, argv, "g")) != -1)
    {
        switch (opt)
        {
            case 'g':
                flag_g = 1;
                break;
            default:
                printf("unavailable option\n");
                return 1;
        }
    }    

    signum = atoi(argv[optind]);
    pid = atoi(argv[optind + 1]);
    //printf("signum: %d  pid = %d\n", signum, pid);
    if (flag_g) pid = -pid;
    if(kill(pid, signum) == -1)
    {
        perror("kill");
        exit(1);
    }

    return 0;
}
