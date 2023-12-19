#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "dirent.h"
#include "sys/stat.h"
#include "ftw.h"
#include "getopt.h"
#include "sys/sysmacros.h"
#include "string.h"

#define BUFSIZE 512

static short oct_arr[9] = {0400, 0200, 0100, 0040, 0020, 0010, 0004, 0002, 0001};
static char perms[10] = "rwxrwxrwx";
static size_t root_len;
int flag_l = 0, flag_a = 0, flag_R = 0;

void list(const char*);
void list_l(const char*);
void print_verbose(struct dirent*);
int dfs(const char*, const struct stat*, int);

int main(int argc, char **argv)
{
    int opt;
    while (-1 != (opt = getopt(argc, argv, "alR")))
    {
        switch (opt)
        {
            case 'a':
                flag_a = 1;
                break;
            case 'l':
                flag_l = 1;
                break;
            case 'R':
                flag_R = 1;
                break;
            default:
                printf("Wrong usage\n");
                return -1;
        }
    }

    int verbose = argc - optind;
    if (verbose == 0)
    {
        if (flag_R) root_len = 1, ftw(".", dfs, 10) ;
        else if (flag_l) list_l(".");
        else list(".");
    }
    else
    {   
        for (int i = optind; i < argc; ++i)
        { 
            if (!flag_R && verbose != 1) printf("%s:\n", argv[i]);  

            if (flag_R) root_len = strlen(argv[i]), ftw(argv[i], dfs, 10);
            else if (flag_l) list_l(argv[i]);
            else list(argv[i]);

            if (!flag_R && verbose != 1) printf("\n");
        }
    }
}

void list(const char* dir_path)
{
    DIR* dir;
    struct dirent *entry;

    if (NULL == (dir = opendir(dir_path))) printf("Directory cannot be opened\n");

    while (NULL != (entry = readdir(dir)))
    {
        if (flag_a || entry->d_name[0] != '.')
            printf("%-15s\t", entry->d_name);
    }

    printf("\n");
    closedir(dir);
}

void get_permission(const struct stat *buf, char *des)
{
    for (int i = 1; i < 11; ++i)
    {
        if (buf->st_mode & oct_arr[i - 1]) des[i] = perms[i - 1];
        else des[i] = '-';
    }

    des[10] = '\0';

    mode_t mask = 070000;
    if ((mask & buf->st_mode) == __S_IFBLK) des[0] = 'b';
    else if ((mask & buf->st_mode) == __S_IFCHR) des[0] = 'c';
    else des[0] = '-';
}

void print_verbose(struct dirent* entry)
{
    struct stat buf;
    char des[11];
    if (0 != stat(entry->d_name, &buf)) return;
    else
    {
        get_permission(&buf, des);
        printf("%-15s\t", des);   
        //printf("%3o\t", buf.st_mode & 0777);
        printf("%3ld\t", buf.st_nlink);
        printf("%6d\t", buf.st_uid);
        printf("%6d\t", buf.st_gid);
        if (des[0] == '-') printf("%12ld\t", buf.st_size);
        else printf("%12d, %d\t", major(buf.st_rdev), minor(buf.st_rdev));
        printf("%-15s\t\n", entry->d_name);
    }
}

void list_l(const char* dir_path)
{
    char pre_dir[BUFSIZ];
    struct stat buf;
    DIR *dir;
    struct dirent *entry;
    if (NULL == getcwd(pre_dir, BUFSIZ)) perror("getcwd error");
    if (NULL == (dir = opendir(dir_path))) printf("Directory cannot be opened\n");
    if (0 != chdir(dir_path)) perror("chdir error");

    while (NULL != (entry = readdir(dir)))
    {
        if (flag_a || entry->d_name[0] != '.')
            print_verbose(entry);
    }

    if (0 != chdir(pre_dir)) perror("chdir error");
    closedir(dir);
}

int dfs(const char* dir_name, const struct stat *sptr, int type)
{
    if (type == FTW_NS || type == FTW_F) return 0;
    if (type == FTW_D)
    {
        if (!flag_a)
        {
            size_t dir_len = strlen(dir_name);
            if (dir_len != root_len) // current directory must be subdirectory
            {
                // the name of subdirecotry is longer than the one of its parent.
                const char *delim = dir_name;
                while (delim = strchr(delim, '/'))
                {
                    if (*(delim + 1) == '.') return 0;
                    delim = delim + 1;
                }
            }
        }        
        printf("%s:\n", dir_name);
        if (flag_l) list_l(dir_name);
        else list(dir_name);

        printf("\n");
    }

    return 0;    
}
