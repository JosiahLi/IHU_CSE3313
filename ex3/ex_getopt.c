#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

int main(int argc, char** argv)
{
    int opt;
    char *add_arg = NULL;
    int flag_b = 0, flag_c = 0;

    while ((opt = getopt(argc, argv, "a:bc")) != -1)
    {
        switch (opt)
        {
            case 'a':
                add_arg = optarg;
                break;
            case 'b':
                flag_b = 1;
                break;
            case 'c':
                flag_c = 1;
                break;
            default:
                printf("Usage: %s [-a <arg>] [-b] [-c]\n", argv[0]);
                break;
        }
    }

    if (add_arg) printf("addtional arg with -a is %s\n", add_arg);
    if (flag_b) printf("-b option is set\n");
    if (flag_c) printf("-c option is set\n");

    return 0;
}