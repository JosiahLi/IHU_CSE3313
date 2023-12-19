#define _GNU_SOURCE

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    pid_t pid;
    int status, exit_status, sig_no;
    pid = fork();

    if (pid == 0)
    {
        printf("[Child] %d: sleep for 20 seconds\n", getpid());
        sleep(20);
        exit(1);
    }

    do
    {
        if ((pid = waitpid(pid, &status, WCONTINUED| WUNTRACED)) == -1)
        {
            perror("wait failed");
            exit(1);
        }   

        if (WIFEXITED(status))
        {
            exit_status = WEXITSTATUS(status);
            printf("Child terminated normally\n");
            printf("Exit status from %d was %d\n", pid, exit_status);
        }
        else if (WIFSIGNALED(status))
        {
            sig_no = WTERMSIG(status);
            printf("Child was terminated by a signal\n");
            printf("Signal number %d terminated child %d\n", sig_no, pid);
        }
        else if (WIFSTOPPED(status))
        {
            sig_no = WSTOPSIG(status);
            printf("Child was stopped by delivery of a signal\n");
            printf("Signal number %d stopped child %d\n", sig_no, pid);
        }
        else if (WIFCONTINUED(status))
        {
            printf("Child was resumed by delivery of a signal\n");
        }

    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    
    return 0;
}