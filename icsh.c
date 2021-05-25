#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>

#define LEN_INPUT 9999
int quitStatus = 1;

void handler(int sig){

      if(sig == SIGTSTP){
        printf("0");
      }
}

int buildInCommand(int sig , char parse[] , char token[]){

    // if(sig == SIGTSTP){
    //     printf("0\n");
    //   }

      char arg[LEN_INPUT];
      strcpy(arg , "/bin/");
      strcat(arg ,parse);
      pid_t pid = fork();

      struct sigaction signal;

      if(pid < 0){printf("Error.\n");exit(EXIT_FAILURE);}

      if(pid == 0){
          signal.sa_handler = SIG_DFL;
          sigaction(SIGINT, &signal, NULL);
          sigaction(SIGTSTP, &signal, NULL);
          pid = getpid();
          setpgid(pid, pid);
          tcsetpgrp(0, pid);

          if(execl(arg,  parse, token, NULL) < 0){
            printf("bad command.\n");
            exit(0);
          }
          else{
            execl(arg, parse, token, NULL);
            exit(0);
          }
      }
      else{
        int childStat;
        setpgid(pid, pid);
        tcsetpgrp(0, pid);
        waitpid(pid , &childStat, WUNTRACED);
        tcsetpgrp(0, getpid());
        if(sig == SIGTSTP){
          printf("%d" , pid);
      }
        return 0;
      }
    }
  


int commands(char **inputLine , char **prevInputLine){ //taking in commands 
    char temp[LEN_INPUT];
    strcpy(temp , *inputLine);

    if(temp[strlen(temp) - 1] == '\n'){temp[strlen(temp) - 1 ] = '\0';}

    if(!strcmp(temp , "echo\n") || !strcmp(temp , "echo ")){printf("\n"); return 0;}

    char *token = strtok(temp , " ");
    token = strtok(NULL , " ");

    if(!strcmp(temp , "echo")){
      while(token != NULL){
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
      return buildInCommand(NULL, temp, token);
    }
        
}

void  getLine(char **inputLine){ //reading input 
    char temp[LEN_INPUT];
    fgets(temp , LEN_INPUT, stdin);
    strcpy(*inputLine , temp);
}

void shellMode(){

    struct sigaction sigint , sigtstp, sigttou;

    sigint.sa_handler = buildInCommand;
    sigtstp.sa_handler = buildInCommand;
    sigttou.sa_handler = SIG_IGN;

    sigaction(SIGINT, &sigint, NULL); 
    sigaction(SIGTSTP, &sigtstp, NULL); 
    sigaction(SIGTTOU, &sigttou, NULL); 

    int status = 0; //life cycle checker
    char *inputLine = malloc(sizeof(char) * LEN_INPUT);
    char *prevInputLine = malloc(sizeof(char) * LEN_INPUT); 
    printf("Starting IC shell\n");
  do{
      sigaction(SIGINT, &sigint, NULL); 
      sigaction(SIGTSTP, &sigtstp, NULL); 
      sigaction(SIGTTOU, &sigttou, NULL); 
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



 
