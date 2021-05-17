#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#define LEN_INPUT 9999
int quitStatus = 1;

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
      char arg[LEN_INPUT];
      strcpy(arg , "/bin/");
      strcat(arg ,temp);
      pid_t pid = fork();
      if(pid < 0){printf("Error.\n");exit(EXIT_FAILURE);}
      if(pid == 0){
        if(execl(arg, temp , token, (char *)0) < 0){
        printf("bad command.\n");
      }
        else{
          execl(arg, inputLine,(char *)0);
        }
        exit(0);
      }
      else{
        int status;
        wait(&status);
        if(WIFEXITED(status)){
          return 0;
        }
        
      }
    }
}

void  getLine(char **inputLine){ //reading input 
    char temp[LEN_INPUT];
    fgets(temp , LEN_INPUT, stdin);
    strcpy(*inputLine , temp);
}

void shellMode(){
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
  exit(status);
}

void main(int argc, char* argv[])
{
    if(argc > 1){
      scriptMode(&argv[1]);
    }
    shellMode();
}



 