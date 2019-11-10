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
#define MAX_CMD_ARG 10
#define BUFSIZE 256

static const char *prompt = "myshell> ";
char* cmdvector[MAX_CMD_ARG];
char  cmdline[BUFSIZE];
struct __jmp_buf_tag to_shell;
struct __jmp_buf_tag to_child_kill;

void fatal(char *);
int makelist(char *, const char *, char** , int);
int is_no_fork(char *[]);
int is_background(char *[], int);
void sig_ign_handler (int);
void sig_dfl_handler(int);
#endif