#include "header.h"
void set_std_fd(){
  if(dup2(stdin_fd, STDIN_FILENO) == -1) fatal("dup2 STDIN");
  if(dup2(stdout_fd,STDOUT_FILENO) == -1) fatal("dup2 STDOUT");
}
void fatal(char *str)
{
  perror(str);
  exit(1);
}
int have_pipe(char* s, int next_cmd){
  if(next_cmd == -1) return -1;
  while(s[next_cmd]){
    if(s[next_cmd] == '|'){
      s[next_cmd] = 0;
      return next_cmd+1;
    }
    next_cmd++;
  }
  return -1;
}
int find_char(const char* s, char key){
  int i=0;
  int pos=-1;
  while(*s){
    if(*s++==key) pos = i;
    i++;
  }
  return pos;
}
int is_delimiter(const char c, const char* delimiters){
    if(c=='&') return 1;
    if(c=='<') return 1;
    if(c=='>') return 1;
  while(*delimiters){
    if(c == *delimiters) return 1;
    delimiters++;
  }
  return 0;
}
int get_lexeme(char *s, const char *delimiters, char* lexeme){
  int start_flag = 0;
  int offset=0;
  while(*s){
    if(start_flag){
      if(is_delimiter(*s, delimiters)) {
        *lexeme = 0;
        return offset;
      }
      *lexeme++ = *s;
    }
    else if(!is_delimiter(*s, delimiters)){
      start_flag = 1;
      *lexeme++ = *s;
    }
    offset++;
    s++;
  }
  *lexeme = 0;
  return offset;
}
int makelist(char *s, const char *delimiters, char **list, int MAX_LIST)
{
  //redirect;
  int in_redirect = find_char(s, '<');
  if(in_redirect != -1){
    in_redirect_flag =1;
    char filename[256];
    int iterator = in_redirect;
    int maximum = in_redirect+get_lexeme(&s[in_redirect+1], delimiters, filename);
    for(;iterator<=maximum; iterator++){
      s[iterator] = ' ';
    }
    int fd = open(filename, O_RDONLY);
    if(fd == -1) {
      perror(filename);
      return -1;
    }
    if(dup2(fd, STDIN_FILENO) == -1) {
      fatal("dup2");
    }
    close(fd);
  }
  else{
    in_redirect_flag = 0;
  }
  int out_redirect = find_char(s, '>');
  if(out_redirect != -1){
    out_redirect_flag = 1;
    char filename[256];
    int iterator = out_redirect;
    int maximum = out_redirect+get_lexeme(&s[out_redirect+1], delimiters, filename);
    for(;iterator<=maximum; iterator++){
      s[iterator] = ' ';
    }
    int fd = open(filename, O_WRONLY | O_CREAT, 0755);
    if(fd == -1) fatal(filename);
    if(dup2(fd, STDOUT_FILENO) == -1) fatal("dup2");
    close(fd);
  }
  else{
    out_redirect_flag = 0;
  }

//parse
  int i = 0;
  int numtokens = 0;
  char *snew = NULL;

  if ((s == NULL) || (delimiters == NULL))
    return -1;

  snew = s + strspn(s, delimiters); /* Skip delimiters */
  if ((list[numtokens] = strtok(snew, delimiters)) == NULL)
    return numtokens;

  numtokens = 1;

  while (1)
  {
    if ((list[numtokens] = strtok(NULL, delimiters)) == NULL)
      break;
    if (numtokens == (MAX_LIST - 1))
      return -1;
    numtokens++;
  }
  return numtokens;
}
int is_no_fork(char *str[])
{
  char tmp[40];
  char tmp2[46];
  int i;
  if (strcmp(str[0], "exit") == 0)
  {
    exit(0);
  }
  else if (strcmp(str[0], "cd") == 0)
  {
    if (str[1] == NULL)
    {
      //if input only "cd"
      getlogin_r(tmp, 40);
      strcpy(tmp2, "/home/");
      strcat(tmp2, tmp);
      if (chdir(tmp2) == -1)
      {
        perror("cd");
      }
    }
    else if (chdir(str[1]) == -1)
    {
      perror("cd");
    }
    return 1;
  }
  return 0;
}
/*
str : vector of command
sz : size of str
*/
int is_background(char *str[], int sz)
{
  int len = strlen(str[sz - 1]);
  if (str[sz - 1][len - 1] == '&')
  {
    //case : ls&
    str[sz - 1][len - 1] = '\0';
    //case : ls &
    if (str[sz - 1][0] == '\0')
    {
      str[sz - 1] = NULL;
    }
    return 1;
  }
  return 0;
}
/*SIGINT, SIGQUIT handler of shell*/
void sig_ign_handler(int signo)
{
  printf("\n");
  longjmp(&to_shell, 1);
}
void sig_dfl_handler(int signo)
{
  printf("\n");
  longjmp(&to_child_kill, 1);
}