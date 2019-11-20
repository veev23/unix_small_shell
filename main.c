#include "header.h"
int main(int argc, char **argv)
{
  extern const char *prompt;
  int i = 0, vector_size, execute_bg;

  //save fd STDIN, STDOUT
  stdin_fd = 1000;
  stdout_fd = 1001;
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

  int pipe_fd[10][2];
  int next_cmd, start_cmd;
  int in_pipe = 0; //had pipe
  int nth_cmd; //n-th cmd in pipe
  while (1)
  {
    //if shell recive SIGINT, SIGQUIT
    __sigsetjmp(&to_shell, 1);
    signal(SIGINT, sig_ign_handler);
    signal(SIGQUIT, sig_ign_handler);

    nth_cmd = 0;
    set_std_fd();
    start_cmd = 0;
    fputs(prompt, stdout);
    fgets(cmdline, BUFSIZ, stdin);
    cmdline[strlen(cmdline) - 1] = '\0';
    next_cmd = have_pipe(cmdline, 0);
    do
    {
      if(nth_cmd > 0){
      dup2(pipe_fd[nth_cmd-1][0], STDIN_FILENO);
      close(pipe_fd[nth_cmd-1][0]);
      }
    if (next_cmd != -1)
    { //if using pipe
      if (pipe(pipe_fd[nth_cmd]) == -1)
        perror("pipe");
      dup2(pipe_fd[nth_cmd][1], STDOUT_FILENO);
      close(pipe_fd[nth_cmd][1]);
      in_pipe = 1;
    }
    else{
      close(STDOUT_FILENO);
      dup2(stdout_fd, STDOUT_FILENO);
    }
      if (next_cmd == -1 && in_pipe)
      {
        if (dup2(stdout_fd, STDOUT_FILENO) == -1)
          fatal("dup2 STDOUT");
      }
      vector_size = makelist(&cmdline[start_cmd], " \t", cmdvector, MAX_CMD_ARG);
      if (vector_size == -1)
        continue;
      start_cmd = next_cmd;
      execute_bg = is_background(cmdvector, vector_size);
      if (is_no_fork(cmdvector))
        continue;
      switch (pid = fork())
      {
      case 0:
      /*if(start_cmd == 0 && in_pipe && !in_redirect_flag){
        close(pipe_fd[0]); close(STDIN_FILENO);
        dup2(stdin_fd, STDIN_FILENO);
      }*/
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
        if (start_cmd == -1 && in_pipe)
        {
          in_pipe = 0;
        }
      }
      nth_cmd ++;
      next_cmd = have_pipe(cmdline, next_cmd);
    } while (in_pipe);
  }
  return 0;
}
