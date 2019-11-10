#include "header.h"
int main(int argc, char **argv)
{
  extern const char *prompt;
  int i = 0, vector_size, execute_bg;
  pid_t pid;
  while (1)
  {
    //if shell recive SIGINT, SIGQUIT
    __sigsetjmp(&to_shell, 1);
    signal(SIGINT, sig_ign_handler);
    signal(SIGQUIT, sig_ign_handler);

    fputs(prompt, stdout);
    fgets(cmdline, BUFSIZ, stdin);
    cmdline[strlen(cmdline) - 1] = '\0';
    vector_size = makelist(cmdline, " \t", cmdvector, MAX_CMD_ARG);
    execute_bg = is_background(cmdvector, vector_size);
    if (is_no_fork(cmdvector))
    {
      continue;
    }
    switch (pid = fork())
    {
    case 0:
      if (execute_bg){
        pid = fork();
        if(pid == -1){
          fatal("main()");
        }
        else if(pid>0){
          //child of shell
          exit(0);
        }
      }
      execvp(cmdvector[0], cmdvector);
      fatal("main()");
    case -1:
      fatal("main()");
    default:
      signal(SIGINT, sig_dfl_handler);
      signal(SIGQUIT, sig_dfl_handler);
      if(__sigsetjmp(&to_child_kill, 1)){
      kill(pid, SIGINT);
      }
      wait(NULL);
    }
  }
  return 0;
}
