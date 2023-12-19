#include "smallsh.h"

char prompt[MAXBUF] = "Command> ";
char home_path[MAXBUF];
size_t home_len;

int main(void)
{
    init_home();
    init_prompt();

    while (userin(prompt) != EOF) procline();

    return 0;
}