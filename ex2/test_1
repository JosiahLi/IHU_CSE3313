#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int fd;
    if (argc != 2)
    {
        printf("Usage: ex_dup <filename>\n");
        return -1;
    }

    fd = open(argv[1], O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror("opening file failed\n");
        return -2;
    }
    else
    {
        printf("Before dup2()\n");
        dup2(fd, 1);
        printf("After, dup2()\n");
    }

    close(fd);

    return 0;
}