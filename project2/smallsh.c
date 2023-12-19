#include "smallsh.h"

extern char prompt[MAXBUF];
extern char home_path[MAXBUF];
extern size_t home_len;
extern pid_t shpid;

static char inpbuf[MAXBUF]; // stores the string of command
static char tokbuf[2 * MAXBUF]; // stores the argument seperated by '\0'
static char* ptr = inpbuf;
static char* tok = tokbuf;
/* buffer for the arguments of echo */
static char *echo_arg[MAXARG] = {"echo", };
/* the keyword volatile tells compiler not to optimize the variables */
volatile int backcode; /* the most current exit code of background processes */
volatile pid_t backpid;
volatile unsigned char forecode; /* the exit code of foreground process */
volatile pid_t foregpid; /* the group id of child running in foreground */

static char special[] = {' ', '\t', '&', ';', '\n', '\0'};

void fatal(const char *msg)
{
    perror(msg);
    exit(1);
}

void init_echo(char **cmd)
{
    int echo_cnt;
    for (echo_cnt = 1; cmd[echo_cnt]; echo_cnt++)
    {
        /* each of the buffers allocated on heap  */
        echo_arg[echo_cnt] = (char *)malloc(MAXBUF);
        strcpy(echo_arg[echo_cnt], cmd[echo_cnt]);
    } 
    echo_arg[echo_cnt] = NULL;
}

void delete_echo(void)
{
    int i;
    for (i = 1; echo_arg[i]; ++i)
    {
        /* deallocate the heap memory */
        free(echo_arg[i]);
        echo_arg[i] = NULL;
    }
}

int userin(char* p) // p is the prompt string
{
    int c, count;
    ptr = inpbuf; // let ptr point to the beginning of input buffer
    tok = tokbuf; // let ptr point to the beginning of 

    printf("%s", p); // print the prompt string
    count = 0; // count for the length of command string

    while (1)
    {
        // First, get a character from stdin (user's command).
        if (EOF == (c = getchar()))
        {
            //if (errno = EAGAIN)
            return EOF; // has read the EOF or an error has occured.
        }
        if (count < MAXBUF) inpbuf[count++] = c; // store the character read from stdin to inpbuf
        if (c == '\n' && count < MAXBUF) // replace 'enter' with zero-terminator
        {
            inpbuf[count] = '\0';
            return count; // return the length of the command
            // Note: right now inpbuff stores unparsed command
        }

        if (c == '\n' || count >= MAXBUF)
        {
            // length overflow
            printf("smallsh: input line too long\n");
            count = 0;
            printf("%s", p);
        }
    }
}

int gettok(char** outptr) // outptr is used for argument vector
{
    int type;
    // tok points to tokbuf
    *outptr = tok; // let outptr point to tokbuf 
    // ptr points to the first character of inpbuf
    while (*ptr == ' ' || *ptr == '\t') ptr++; // clear the space and tab
    
    *tok++ = *ptr; // equivalent to *tok = *ptr and tok++;

    switch (*ptr++)
    {
        case '\n':
            type = EOL; // the command begins with 'enter', just ignore it
            break;
        case '&':
            type = AMPERSAND;
            break;
        case ';':
            type = SEMICOLON;
            break;
        default:
            type = ARG;
            while (inarg(*ptr)) *tok++ = *ptr++;
    }

    *tok++ = '\0'; // set the end of tokbuf to '\0'
    return type;
}

int inarg(char c) // check whether c is a special character, 1 is, otherwise 0
{
    char *wrk;

    for (wrk = special; *wrk; ++wrk) 
        if (c == *wrk) return 0;

    return 1;
}

void procline()
{
    char *arg[MAXARG + 1]; // argument vector used for exec
    int toktype, type;
    int narg = 0;
    while (1)
    {
        // parse command line stored in inpbuf and store it to arg
        switch (toktype = gettok(&arg[narg]))
        {
            case ARG:
                if (narg < MAXARG) narg++;
                break;
            case EOL:
            case SEMICOLON:
            case AMPERSAND:
                if (toktype == AMPERSAND) type = BACKGROUND;
                else type = FOREGROUND;

                if (narg != 0)
                {
                    arg[narg] = NULL;
                    runcommand(arg, type);
                }    
                // Since the next to last of character in inpbuf is '\n', the next line must be executed
                if (toktype == EOL) return; 

                narg = 0;
                break;
        }
    }
}

void init_home(void)
{
    struct passwd *ptr;
    /* 
        Use getpwuid to  get the home directory of the user
     */
    if ((ptr = getpwuid(getuid())) == NULL)
    {
        perror("getpwuid");
        return;
    }
    /* 
        copy user's home directory to the buffer defined in main.c
    */ 
    if (NULL == strcpy(home_path, ptr->pw_dir))
    {
        perror("strcpy");
        return;
    }
    /* store the length of home directory */
    home_len = strlen(home_path);
}

void CD(char** argv)
{
    /* cd command works for only argument */
    if (argv[2] != NULL) 
    {
        printf("smallsh: cd: too many arguments\n");
        return;
    }
    /* go to home directory if there is no argument */
    if (argv[1] == NULL)
    {
        /* use to buffered string */
        if (chdir(home_path) < 0)
        {
            perror("chdir");
            return;
        }
        /*
            the working directory is changed, 
            so we have to initialized the prompt again
        */
        init_prompt();
        
        return;
    } 
    /* when it has only one argument */
    // the first character is '~', which implies home dirctory
    if (argv[1][0] == '~' && (argv[1][1] == '/' || argv[1][1] == '\0')) 
    {
        /*
            Because a direcotry name can begin with a character '~' 
            which has been defined for home directory, so we have to check
            whether the directory in the path begins with a '~'.
            Or equavalently, we just need to check its comtrapositive,
            this is, if the argument begins with '~' followed by '/' or '\0',
            then it must be home directory.
        */
        size_t len = strlen(argv[1]);
        if (chdir(home_path) < 0) 
        {
            perror("chdir");
            return;
        }
        /*
            if len equals 1 or 2, then the argument might be "~" or "~/". 
            Thus we can get into home directly.
        */
        if (len > 2) // if len is greater than 2, then the directory is a child of home
        {
            /* the first character of the sub directory begins at address argv[1] + 2 */
            if (chdir(argv[1] + 2) < 0)
            {
                perror("chdir");
                return;
            }
        }
    }
    else if (chdir(argv[1]) < 0)
    {
        /*
            In this case, the argument does not contains `~`
        */
        perror("chdir");
        return;
    }
    /* update the prompt string */
    init_prompt();
}

int is_builtin(char** command) // check if the command is builtin, if so run it 
{
    if (!strcmp(*command, "cd"))
    {
        CD(command);
        return 1;
    }
    else if (!strcmp(*command, "exit")) exit(0);

    return 0; /* not a built-in command */
}

int check_command(char **arg, int *pos)
{
    /* traverse the argument vector */
    for (int i = 0; arg[i]; ++i)
    {
        if (arg[i][0] == '>')
        {
            /* If '>' exists then mark the postion */
            *pos = i;
            return REDIRECTION;
        } 
        else if (arg[i][0] == '|')
        {
            #ifdef DEBUG
            printf("Pipe command\n");
            #endif

            *pos = i;
            return PIPE;
        }
    }

    return 0;
}

void execvp_redirect(char** arg, int pos)
{
    int infd;
    // set null terminator of argument vector
    arg[pos] = NULL;
    /* 
        arg[pos + 1] is the file to which std is redirected
        and open the file, if it exists then truncate it.
    */
    if ((infd = open(arg[pos + 1], O_CREAT | O_TRUNC | O_WRONLY, 0664)) == -1)
    {
        perror("open");
        return;
    }
    
    dup2(infd, 1); // let standard output point to the file 
    close(infd);
    /* 
        right now all output of the process is redirected to the file 
        and the open file discreptors are not closed after loading and running a 
        new program.
    */
    execvp(*arg, arg);
    perror(*arg);
    exit(1);
}

int runcommand(char** cline, int where)
{
    pid_t pid;
    int status, builtin, command_type, position;
    sigset_t blockset, oldset;

    // check whether the commmd is built-in
    builtin = is_builtin(cline);
    if (builtin != 0) return 0; /* not need to fork */
    /* check command type */
    command_type = check_command(cline, &position);
    
    if (!strcmp(cline[0], "echo")) 
    {
        /* initialize the buffer for echo */
        init_echo(cline);
        parse_echo(echo_arg);
        /*
            Since the command string is allocated on .bss section,
            we can safaly let cline point to the heap memory without any memory leak
        */
        cline = echo_arg;
    }
    /*
        To reap all children running in background,
        we have to let the program block signal SIGCHLD
        so that the child running in is reapd by wait function,
        but all children running in background are reaped by signal handler
    */
    /* block signal SIGCHLD */
    if (sigemptyset(&blockset)) 
        fatal("sigempty");
    if (sigaddset(&blockset, SIGCHLD))
        fatal("sigaddset");
    if (sigprocmask(SIG_BLOCK, &blockset, &oldset))
        fatal("sigprocmask [SIG_BLOCK]");
    
    switch (pid = fork())
    {
        case -1:
            perror("smallsh");
            return -1;
        case 0:
            /* 
                Note: child inherits block set of its parent,
                so we must restore its block signal set 
            */
            if (sigprocmask(SIG_SETMASK, &oldset, NULL))
                fatal("sigprocmask [SIG_SETMASK] in child");
            /*
                Furthermore, the group id of the process inherits
                its parent's one. To sent SIGINT to the processes running
                in foreground, we have to seperate such processes from its
                parent. Or, we will terminate the small shell when we press
                Ctrl + C.
            */
            setpgid(0, 0); /* let the process be the group leader */

            /* check whether the command is intended for redirection */
            switch (command_type)
            {
                case REDIRECTION:
                    execvp_redirect(cline, position);
                case PIPE:
                    execvp_pipe(cline, position);
                default:
                    execvp(*cline, cline);
                    perror(*cline);
                    exit(1);
            }
    }
    /* 
        store the group id of the process running in foregroud 
        for sending SIGINT to the group.
    */
    foregpid = pid; 

    if (where == BACKGROUND) // not need to wait the termination of its child
    {
        sigprocmask(SIG_SETMASK, &oldset, NULL);
        backpid = pid;
        printf("[Process id] %d\n", pid);
        return 0;
    }
    // foreground: must wait until the foreground process terminates
    if (waitpid(pid, &status, 0) == -1) return -1;
    else
    {
        /*
            the child running in foreground has been reaped,
            so restore the block set
        */
        sigprocmask(SIG_SETMASK, &oldset, NULL);
        /* get exit status */
        if (WIFEXITED(status)) forecode = WEXITSTATUS(status);
        /*
            Since the group running in foreground has terminated,
            we clear its group id to 0.
        */
        foregpid = 0;
        delete_echo();
        /* deallocate heap memory allocated for echo */
        
        return status;
    }
}

void init_prompt()
{
    /* Store the current directory path in global variable prompt */
    if (NULL == getcwd(prompt, MAXBUF))
    {
        perror("getcwd");
        return;
    }
    /* get the length of current directory 
       used to compare with length of home directory path
    */
    size_t prompt_len = strlen(prompt);
    if (prompt_len >= home_len)
    {
        /* 
            if the length of current directory path is greater or equal to
            the one of home directory path, in which case It might include the
            home directory, so we have to compare the prefix of current directory path 
            with home directory path
         */
        size_t i;
        for (i = 0; i < home_len; ++i)
            if (home_path[i] != prompt[i]) break;
        /*  current directory path contains home directory path
            only when i equals to home_len 
        */
        if (i == home_len) // prompt contains home path
        {
            /*
                current direcotry is home directory if prompt_len equals to home_len,
                so we just replace the current directory path with ~.
                Otherwise we reformat the prompt string by using sprintf
            */
            if (prompt_len == home_len) sprintf(prompt, "~$ ");
            else sprintf(prompt, "~%s$ ", prompt + home_len); 
            
            return;
        }
    }    
    /* 
        In this case, current directory path can not contain home directory path,
        so we don't need to replace any path, just append ‘$’. 
     */
    prompt[prompt_len] = '$';
    prompt[prompt_len + 1] = ' ';
    prompt[prompt_len + 2] = '\0';

}

void child_handler(int sig)
{
    /* save the old error number */
    int olderror = errno;

    int status;
    /* reap all children */
    while (waitpid(-1, &status, WNOHANG) > 0)
    {
        if (WIFEXITED(status)) backcode = WEXITSTATUS(status);

        #ifdef DEBUG
        printf("reaped\n");
        #endif
    }
    /* restore the old error number */
    errno = olderror;
}

void int_handler(int sig)
{
    if (!foregpid) /* there is no process running in foreground */
    {
        printf("\n%s", prompt);
        fflush(stdout);
        return;
    }

    int olderror = errno;
    /* send signal SIGINT to the group of the procss running in the foreground */
    if (kill(-foregpid, SIGINT)) fatal("kill [int_handler]");

    #ifdef DEBUG
    printf("send SIGINT\n");
    #endif

    puts("");
    errno = olderror;
}
/* only child of small shell can call this function */
void execvp_pipe(char** argv, int position)
{
    #ifdef DEBUG
    printf("execvp_pipe\n");
    #endif
    /*
        right now, the signal SIGCHLD is blocked
    */
    int p[2]; /* used for pipe */
    char **cmd1, **cmd2;
    pid_t pid;
    argv[position] = '\0'; /* use '\0' to seperate the command */
    /* 
        cmd1 is used for write end
        cmd2 is used for read end
     */
    cmd1 = argv, cmd2 = argv + position + 1; /* split the command */

    if (pipe(p)) fatal("pipe"); /* get file descriptor for pipes */

    switch (pid = fork())
    {
        case -1:
            fatal("fork in execvp_pipe");
        case 0:
            dup2(p[1], 1);
            /* 
                since stdin is pointing to write end of the pipe, 
                we can close the file descriptors to avoid memory leak. 
            */
            close(p[1]), close(p[0]);
            execvp(cmd1[0], cmd1);
            fatal("pipe write end");
        default:
            waitpid(pid, NULL, 0);
            dup2(p[0], 0);
            close(p[0]), close(p[1]); /* To avoid memory leak */
            execvp(cmd2[0], cmd2);
            fatal("pipe read end");
    }
}

void substitute_string(char* target, const char *substr, const char *replace)
{
    static char bufstr[MAXBUF];
    char *pos = strstr(target, substr);
    while (pos)
    {
        size_t prelen = pos - target;
        size_t sublen = strlen(pos + strlen(substr)); 
        strncpy(bufstr, target, prelen);
        strcpy(bufstr + prelen, replace);
        strcpy(bufstr + prelen + strlen(replace), pos + strlen(substr));
        //printf("bufstr = %s\n", bufstr);
        sprintf(target, "%s", bufstr);
        pos = strstr(target, substr);
    }

    //printf("target = %s\n", target);
}

void parse_echo(char **argv)
{
    int i, j;
    static char forestr[4];
    static char backstr[16] = "";
    static char shstr[16];
    /* initialize the strings */
    sprintf(forestr, "%u", forecode);
    if (backpid) sprintf(backstr, "%d", backpid);
    sprintf(shstr, "%d", shpid);
    
    //printf("%s %s %s\n", forestr, backstr, shstr);

    for (i = 1; argv[i]; ++i)
    {
        substitute_string(argv[i], "$?", forestr);
        substitute_string(argv[i], "$!", backstr);
        substitute_string(argv[i], "$$", shstr);
    }

}