#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#define BUFSIZE 64

int main(int argc, char** argv)
{
    char buf[BUFSIZ];
    int fd;
    if (argc != 2)
    {
        printf("Usage: %s <FIFO NAME>\n", argv[0]);
        exit(1);
    }

    if (mkfifo(argv[1], 0666) == -1)
    {
        if (errno != EEXIST)
        {
            perror("mkfifo");
            exit(1);
        }
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0)
    {
        perror("open");
        exit(1);
    }

    while (read(fd, buf, BUFSIZE) > 0)
    {
        printf("Msg received: %s\n", buf);
    }

    remove(argv[1]);

    return 0;
}