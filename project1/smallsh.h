#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>

#define EOL 1
#define ARG 2
#define AMPERSAND 3
#define SEMICOLON 4

#define MAXARG 512
#define MAXBUF 512

#define FOREGROUND 0
#define BACKGROUND 1

#define REDIRECTION 1 

int userin(char *);
void procline();
int gettok(char**);
int inarg(char);
int runcommand(char**, int);
int is_builtin(char**);
void CD(char**);
void init_prompt();
void init_home();
int check_command(char**, int*);
void execvp_redirect(char**, int);