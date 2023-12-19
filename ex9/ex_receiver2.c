#define _GNU_SOURCE
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/select.h>
#include <setjmp.h>

#define MAXSIZE 64

fd_set readset;
int cntfd = 3; /* how many open file descriptors */
sigjmp_buf position;

void ERROR(const char *msg)
{
    perror(msg);
    exit(1);
}

void usr1_handler(int signo, siginfo_t *siginfo, void *param2)
{
    static char fifo_name[MAXSIZE];
    int fd;
    sprintf(fifo_name, "FIFO_%d", siginfo->si_pid);
    printf("%s\n", fifo_name);

    if ((fd = open(fifo_name, O_RDONLY)) == -1)
    {
        printf("usr1_handler open failed\n");
        exit(1);
    } 
    FD_SET(fd, &readset);
    if (fd >= cntfd) cntfd = fd + 1;
    siglongjmp(position, 1);
}


int main(int argc, char **argv)
{
    int nread;
    struct sigaction act = {.sa_sigaction = usr1_handler};
    act.sa_flags = SA_SIGINFO; /* tell kernal that we want to use an extended signal handler */
    sigfillset(&act.sa_mask);     
    sigaction(SIGUSR1, &act, NULL);

    char buf[MAXSIZE];
    printf("PID: %d\n", getpid());

    FD_ZERO(&readset);
    FD_SET(STDIN_FILENO, &readset);
    fd_set temp; /* used for store the temporary value of readset */
    /*
        make use of sigsetjpm to record the context,
        since we need to update readset whenever a sender connect to the process
    */
    sigsetjmp(position, 1);

    while (temp = readset, select(cntfd, &temp, NULL, NULL, NULL) > 0)
    {
        if (FD_ISSET(STDIN_FILENO, &temp))
        {
            nread = read(STDIN_FILENO, buf, MAXSIZE);

            if (!nread) continue;

            buf[nread - 1] = '\0'; 
            printf("received message(stdin): %s\n", buf);
        }

        if (cntfd == 3) continue;
        for (int i = 3; i < cntfd; ++i)
            if (FD_ISSET(i, &temp))
            {
                nread = read(i, buf, MAXSIZE);
                buf[nread] = '\0';

                if (!nread)
                {
                    close(i);
                    FD_CLR(i, &readset);
                }

                printf("received message new: %s\n", buf);
                memset(buf, 0, MAXSIZE);
                fflush(stdout);
            }
    }

    return 0;
}
