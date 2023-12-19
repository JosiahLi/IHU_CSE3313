#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#define BUFSIZE 64
int main() 
{
    int ptc_fd[2]; /* Parent to Child pipe */
    int ctp_fd[2]; /* Child to Parent pipe */
    char buf[BUFSIZE];
    if(pipe(ptc_fd) == -1) 
    {
        perror("parent to child pipe");
        return 0;
    }
    if(pipe(ctp_fd) == -1) 
    {
        perror("child to parent pipe");
        return 0;
    }

    switch(fork()) 
    {
        case -1:
            perror("fork");
            return 0;
        case 0:
            close(ctp_fd[0]);
            close(ptc_fd[1]);
            write(ctp_fd[1], "Hello, I'm child.", BUFSIZE);
            read(ptc_fd[0], buf, BUFSIZ);
            printf("Message from parent: %s\n", buf);
            return 0;
        default:
            close(ptc_fd[0]);
            close(ctp_fd[1]);
            read(ctp_fd[0], buf, BUFSIZE);
            printf("Message from child: %s\n", buf);
            sleep(5);
            write(ptc_fd[1], "Hello, I'm your parent.",BUFSIZE);
            wait(NULL);
            return 0;
    }
}