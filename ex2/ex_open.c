#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define PERMS 0644

int main()
{
    int fd;

    if ((fd = open("testfile", O_WRONLY | O_CREAT | O_EXCL)) == -1)
    {
        perror("Couldn't open testfile");
        return -1;
    }

    printf("Open testfile succeed!\n");

    close(fd);

    return 0;
}