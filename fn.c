#include "header.h"
void fatal(char *str)
{
  perror(str);
  exit(1);
}

int makelist(char *s, const char *delimiters, char **list, int MAX_LIST)
{
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
      if(chdir(tmp2)==-1){
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
void sig_ign_handler (int signo){
  printf("\n");
  longjmp(&to_shell, 1);
}
void sig_dfl_handler(int signo){
  printf("\n");
  exit(0);
}