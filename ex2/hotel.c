#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define LENGTH 11
#define NROOMS 10

char namebuf[LENGTH];
int infile = -1;
char* getoccupier(int);

int main(int argc, char *argv[])
{
    char *p;
    if (argc < 2)
    {
        printf("Usage: %s <roomnum a>, <roomnum b>, ...\n", argv[0]);
        return -1;
    }

    for (int i = 1; i < argc; ++i)
    {
        int roomnum = atoi(argv[i]);
        if (p = getoccupier(roomnum)) printf("Room %2d, %s\n", roomnum, p);
        else
        {
            printf("Error on room %d", roomnum);
            perror("");
        }
    }

    return 0;
}

char* getoccupier(int roomnum)
{
    off_t offset;
    ssize_t nread;

    if (infile == -1 && (infile = open("residents", O_RDONLY)) == -1) return (NULL);

    offset = (roomnum - 1) * 11;
    lseek(infile, (off_t)offset, SEEK_SET);
    nread = read(infile, namebuf, LENGTH);

    if (nread <= 0) return (NULL);

    namebuf[nread - 1] = '\0';

    return namebuf;

}