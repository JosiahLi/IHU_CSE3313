#define _GNU_SOURCE
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#define MAXSIZE 64

char fifo_name[MAXSIZE];
int fd;

int ERROR(const char* str)
{
    perror(str);
    exit(1);
}

void int_handler(int signo)
{
    close(fd);
    if (unlink(fifo_name) != 0) ERROR("unlink");
    exit(0);
}

int main(int argc, char **argv)
{
    char buf[MAXSIZE];
    int nread;
    //int nread;
    pid_t pid_receiver;
    if (argc < 2)
    {
            printf("Usage: %s <RECEIVER PID>\n", argv[0]);
            exit(1);
    } 
    /* register signal handler */
    struct sigaction act = {.sa_handler = int_handler};
    sigfillset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);

    sprintf(fifo_name, "FIFO_%d", getpid());
    mkfifo(fifo_name, 0666);
    printf("FIFO_NAME: %s\n", fifo_name);
    
    if ((pid_receiver = atoi(argv[1])) == -1) ERROR("atoi");
    if (kill(pid_receiver, SIGUSR1) == -1) ERROR("kill");

    if ((fd = open(fifo_name, O_WRONLY)) == -1)
    {
        printf("open %s failed\n", fifo_name);
        exit(1);
    }

    while ((nread = read(STDIN_FILENO, buf, MAXSIZE)) > 0)
    {
        if (write(fd, buf, nread - 1) != nread - 1) ERROR("write to fifo");
    }

    perror("stdin read");

    return 1;
}