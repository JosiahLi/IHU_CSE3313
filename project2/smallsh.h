#define _GNU_SOURCE
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>

#define EOL 1
#define ARG 2
#define AMPERSAND 3
#define SEMICOLON 4

#define MAXARG 512
#define MAXBUF 512

#define FOREGROUND 0
#define BACKGROUND 1

#define REDIRECTION 1 
#define PIPE 2

void fatal(const char*);
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
void execvp_pipe(char**, int);
void child_handler(int);
void int_handler(int);
void substitute_string(char*, const char*, const char*);
void parse_echo(char **);
void init_echo(char **);
void delete_echo();
