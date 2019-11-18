#include "header.h"
int main(int argc, char **argv)
{
  extern const char *prompt;
  int i = 0, vector_size, execute_bg;

  //save fd STDIN, STDOUT
  if (dup2(STDIN_FILENO, stdin_fd) == -1)
    fatal("dup2 STDIN");
  if (dup2(STDOUT_FILENO, stdout_fd) == -1)
    fatal("dup2 STDOUT");

  struct sigaction chld_act;

  chld_act.sa_handler = SIG_IGN;
  sigemptyset(&(chld_act.sa_mask));
  chld_act.sa_flags = SA_NOCLDWAIT;
  sigaction(SIGCHLD, &chld_act, NULL);

  pid_t pid;
  while (1)
  {
    //if shell recive SIGINT, SIGQUIT
    __sigsetjmp(&to_shell, 1);
    signal(SIGINT, sig_ign_handler);
    signal(SIGQUIT, sig_ign_handler);

    set_std_fd();
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
        execvp(cmdvector[0], cmdvector);
        fatal(cmdvector[0]);
      case -1:
        fatal("fork()");
      default:
        signal(SIGINT, sig_dfl_handler);
        signal(SIGQUIT, sig_dfl_handler);
        //foreground process kill
        if (__sigsetjmp(&to_child_kill, 1))
        {
          kill(pid, SIGINT);
        }
        if (!execute_bg)
        {
          waitpid(pid, NULL, 0);
        }
      }
    
  }
  return 0;
}
