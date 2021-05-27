#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>

#define LEN_INPUT 9999
int quitStatus = 1;
int childExitCode;


int buildInCommand(char parse[]){
      char filename[LEN_INPUT];
      strcpy(filename , parse);
      char *arg[LEN_INPUT];
      char *token = strtok(parse , " ");
      int idx = 0;
      int pointers;


      token = strtok(NULL , " ");
      while(token != NULL){
        if(!strcmp(token , "\n")){arg[idx] = NULL;}
        else{arg[idx] = token;}
        token = strtok(NULL , " ");
        idx++;
        pointers = idx;
      }

      char toFile[LEN_INPUT];//extracting file name
      int dupCheck = 0; //check if we have to duplicate/redirect 
      int bgCheck = 0;

      for(idx = 0; idx < pointers-1; idx++){ //finding dup | bg checker 
        if(arg[idx] == NULL ){break;}
          if(strchr(arg[idx] , '>')){
            arg[idx] = NULL;
            strcpy(toFile , arg[idx+1]);
            toFile[strlen(toFile) - 1] = '\0';
            arg[idx+1] = NULL;
            dupCheck = 1;
            continue;
          }

        if (strchr(arg[idx] , '&')){
            arg[idx] = NULL;
            arg[idx+1] = NULL;
            bgCheck = 1;
            continue;
          }
      }

      free(token);

      if(parse[strlen(filename)-1] == '\n'){parse[strlen(filename)-1] = '\0';}

      pid_t pid = fork();

      struct sigaction sig;


      if(pid < 0){printf("Error.\n");exit(EXIT_FAILURE);}

      if(pid == 0){
          sig.sa_handler = SIG_DFL;
          sigaction(SIGINT, &sig, NULL);
          sigaction(SIGTSTP, &sig, NULL);
          pid = getpid();
          if(bgCheck){
            setpgid(0, 0);
            tcsetpgrp(0, pid);
          }
          else{
            setpgid(pid, pid);
            tcsetpgrp(0, pid);
          }
          if(dupCheck){
            int file = open(toFile, O_WRONLY | O_CREAT | O_TRUNC , 0777);
            int from = dup2(file , STDOUT_FILENO);
            close(file);
          }
          execlp(parse, parse, arg[0], arg[1], arg[2], NULL);
          printf("bad command\n");
          exit(0);
          }

      else{
        int childStat;
        if(bgCheck){
            setpgid(0, 0);
            tcsetpgrp(0, pid);
            waitpid(pid , &childStat, WNOHANG);
          }
          else{
            setpgid(pid, pid);
            tcsetpgrp(0, pid);
             waitpid(pid , &childStat, WUNTRACED);
          }

        tcsetpgrp(0, getpid());
        if(!WIFEXITED(childStat)){printf("\n");}
        if(WIFSIGNALED(childStat)){childExitCode = WTERMSIG(childStat) + 128;}
        if(WIFSTOPPED(childStat)){childExitCode = WSTOPSIG(childStat) + 128;}
        arg[0] = NULL; arg[1] = NULL; arg[2] = NULL;
        return 0;
      }
    }
  


int commands(char **inputLine , char **prevInputLine){ //taking in commands 

    char temp[LEN_INPUT];
    char temp2[LEN_INPUT];
    strcpy(temp , *inputLine);
    strcpy(temp2 , *inputLine);

    if(temp[strlen(temp) - 1] == '\n'){temp[strlen(temp) - 1 ] = '\0';}

    if(!strcmp(temp , "echo\n") || !strcmp(temp , "echo ")){printf("\n"); return 0;}

    char *token = strtok(temp , " ");
    if(!strcmp(temp , "echo")){
      token = strtok(NULL , " ");
      while(token != NULL){
        if(!strcmp(token , "$?")){printf("%d" , childExitCode); return 0;}
        printf("%s " ,token);
        token = strtok(NULL , " ");
      }
      printf("\n");
      return 0;
    }

    if(!strcmp(temp , "exit")){
     quitStatus = 0;
     printf("bye\n");
     return (u_int8_t)atoi(token);
    }

    if(!strcmp(temp , "!! ") || !strcmp(temp , "!!\n") || !strcmp(temp , "!!")){
     return commands(&prevInputLine, &inputLine);
    }
    else{
      return buildInCommand(temp2);
    }
        
}

void  getLine(char **inputLine){ //reading input 
    char temp[LEN_INPUT];
    fgets(temp , LEN_INPUT, stdin);
    strcpy(*inputLine , temp);
}

void shellMode(){

    struct sigaction sig;

    sig.sa_handler = SIG_IGN; //go to handler to check which pid is running on back ground
    
    sigaction(SIGINT, &sig, NULL); 
    sigaction(SIGTSTP, &sig, NULL); 
    sigaction(SIGTTOU, &sig, NULL);
    
    int status = 0; //life cycle checker
    char *inputLine = malloc(sizeof(char) * LEN_INPUT);
    char *prevInputLine = malloc(sizeof(char) * LEN_INPUT); 
    printf("Starting IC shell\n");
  do{
      printf("icsh $ ");
      getLine(&inputLine);
      if(!strcmp(inputLine , "\n")){
        continue;
      }
      else{
        status = commands(&inputLine , prevInputLine);
        strcpy(prevInputLine , inputLine);
      }
      
  } while(quitStatus);
  free(inputLine);
  free(prevInputLine);
  exit(status);
    
}

void scriptMode(char **dir){
  FILE *fileName;
  if(!strstr(*dir , ".sh")){printf("Not .sh file.\n");exit(EXIT_FAILURE);}
  fileName = fopen(*dir , "r");
  char *line = malloc(sizeof(char) * LEN_INPUT);
  char *prevLine = malloc(sizeof(char) * LEN_INPUT);
  int status;
  if(fileName == NULL){
    printf("Error opening file.\n");
    exit(EXIT_FAILURE);
  }
  while(fgets(line , LEN_INPUT , fileName) != NULL){
    if(quitStatus == 1){
      status = commands(&line , prevLine);
      strcpy(prevLine , line);
    }
  }
  if(quitStatus == 0){ free(line); free(prevLine); exit(status);}
}

void main(int argc, char* argv[])
{
  
    if(argc > 1){
      scriptMode(&argv[1]);
    }
    shellMode();
}



 
