#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(void)
{
    int fd;
    if ((fd = open("file1", O_WRONLY | O_CREAT, 0777)) == -1)
    {
        perror("open failed");
        return 1;
    }
    close(fd);

    mode_t default_mask = umask(0);

    char s[10];
    for (int i = 0; i < 9; ++i) s[i] = '-';
    s[9] = '\0';

    if (default_mask & S_IRUSR) s[0] = 'r';
    if (default_mask & S_IWUSR) s[1] = 'w';
    if (default_mask & S_IXUSR) s[2] = 'x';
    if (default_mask & S_IRGRP) s[3] = 'r';
    if (default_mask & S_IWGRP) s[4] = 'w';
    if (default_mask & S_IXGRP) s[5] = 'x';
    if (default_mask & S_IROTH) s[6] = 'r';
    if (default_mask & S_IWOTH) s[7] = 'w';
    if (default_mask & S_IXOTH) s[8] = 'x';

    printf("%s\n", s);

    return 0;
}