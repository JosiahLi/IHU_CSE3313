#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>

void print_usage(const char* str)
{
    printf("Default usage: %s <mode> <file name> \n", str);
    printf("-- Change the permission of the <file name> to <mode>\n");
    printf("--additional options:\n");
    printf(" -v: output a diagnostic for every file processed\n");
    printf(" -h: output a usage of ex_chmod\n");  
}

int main(int argc, char** argv)
{
    // -v -h
    int opt;
    int flag_v = 0, flag_h = 0;

    while ((opt = getopt(argc, argv, "vh")) != -1)
    {
        switch (opt)
        {
            case 'v':
                flag_v = 1;
                break;
            case 'h':
                flag_h = 1;
                break;
            default:
                print_usage(argv[0]);
                return -1;
        }
    }

    if (flag_h) 
    {
        print_usage(argv[0]);
        return 0;
    }

    int cnt = argc - optind;
    if (cnt != 2)
    {
        printf("Missing operand\n");
        printf("Try \'ex_chmod -h\' for more information\n");
        return -1;
    }

    mode_t newmode;
    if (access(argv[optind + 1], F_OK) != 0)
    {
        perror("file is not existed");
        return -1;
    }

    sscanf(argv[optind], "%o", &newmode);

    if (chmod(argv[optind + 1], newmode) != 0)
    {
        perror("mode chande failed");
        return -2;
    }

    if (flag_v) printf("mode of %s is changed to %o\n", argv[optind + 1], newmode);
        
    return 0;
}