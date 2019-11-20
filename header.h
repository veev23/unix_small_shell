#ifndef HEADER_H
#define HEADER_H
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <setjmp.h>
#include <fcntl.h>
#define MAX_CMD_ARG 10
#define BUFSIZE 256

int stdin_fd;
int stdout_fd;

int out_redirect_flag;
int in_redirect_flag;

static const char *prompt = "myshell> ";
char* cmdvector[MAX_CMD_ARG];
char  cmdline[BUFSIZE];
struct __jmp_buf_tag to_shell;
struct __jmp_buf_tag to_child_kill;

//set stdin_fd, stdout_fd
void set_std_fd();
void fatal(char *);
//reutrn : '|' next index
int have_pipe(char* s, int next_cmd);
int find_char(const char* s, char key);
int is_delimiter(const char c, const char* delimiters);
//return : lexeme last index + 1
int get_lexeme(char* s, const char *delimiters, char* lexeme);
int makelist(char *, const char *, char** , int);
int is_no_fork(char *[]);
int is_background(char *[], int);
void sig_ign_handler (int);
void sig_dfl_handler(int);
#endif