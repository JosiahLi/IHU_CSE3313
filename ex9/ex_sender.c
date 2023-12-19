#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 64

int main(int argc, char **argv)
{
    int fd;

    if (argc < 3)
    {
        printf("Usage: %s <FIFO NAME> <MESSAGE>\n", argv[0]);
        exit(1);
    }

    if ((fd = open(argv[1], O_WRONLY | O_NONBLOCK)) < 0)
    {
        perror("open");
        exit(1);
    }

    for (int i = 2; i < argc; ++i)
    {
        if (strlen(argv[i]) > BUFSIZE)
        {
            printf("message too long %s\n", argv[i]);
            continue;
        }

        if (write(fd, argv[i], BUFSIZE) == -1)
        {
            perror("sender write");
            exit(1);
        }

        //sleep(1);
    }
    close(fd);

    return 0;
}