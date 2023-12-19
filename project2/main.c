#include "smallsh.h"

char prompt[MAXBUF] = "Command> ";
char home_path[MAXBUF];
size_t home_len;
pid_t shpid; /* used to implement echo $$ */

int main(void)
{
    shpid = getpid();    
    init_home();
    init_prompt();
    /* regist SIGCHLD handler */
    struct sigaction act_chld = {.sa_handler = child_handler};
    act_chld.sa_handler = child_handler;
    /* 
        Note: we have to turn on the flag SA_RESTART
        so that system calls can not be interruped by signals.
     */
    act_chld.sa_flags = SA_RESTART;
    /* blocking all signals whlie chlid handler is executing */
    sigfillset(&act_chld.sa_mask);
    sigaction(SIGCHLD, &act_chld, NULL);
    /* regist SIGINT handler */
    struct sigaction act_int = {.sa_handler = int_handler};
    act_int.sa_flags = SA_RESTART;
    sigfillset(&act_int.sa_mask);
    sigaction(SIGINT, &act_int, NULL);

    while (userin(prompt) != EOF) procline();

    #ifdef DEBUG
    printf("Bye!\n");
    #endif

    return 0;
}