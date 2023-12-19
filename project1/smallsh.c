#include "smallsh.h"

extern char prompt[MAXBUF];
extern char home_path[MAXBUF];
extern size_t home_len;

static char inpbuf[MAXBUF]; // stores the string of command
static char tokbuf[2 * MAXBUF]; // stores the argument seperated by '\0'
static char* ptr = inpbuf;
static char* tok = tokbuf;

static char special[] = {' ', '\t', '&', ';', '\n', '\0'};

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
        if (EOF == (c = getchar())) return EOF; // has read the EOF or an error has occured.
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

    memset(arg, 0, sizeof(arg));

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
    // check whether the commmd is built-in
    builtin = is_builtin(cline);
    if (builtin != 0) return 0; /* not need to fork */

    switch (pid = fork())
    {
        case -1:
            perror("smallsh");
            return -1;
        case 0:
            /* check whether the command is intended for redirection */
            switch (command_type = check_command(cline, &position))
            {
                case REDIRECTION:
                    execvp_redirect(cline, position);
                default:
                    execvp(*cline, cline);
                    perror(*cline);
                    exit(1);
            }
    }

    if (where == BACKGROUND) // not need to wait the termination of its child
    {
        printf("[Process id] %d\n", pid);
        return 0;
    }
    // foreground: must wait until the foreground process terminates
    if (waitpid(pid, &status, 0) == -1) return -1;
    else return status;
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