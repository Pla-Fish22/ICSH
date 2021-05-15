#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int modeCheck; //since script mode and shell mode will work differently
int LEN_INPUT = 1024;

int commands(char **inputLine){ //taking in commands 
    char cmd[1024];
    char dest[1024];
    strcpy(dest , "\n");
    if(!strchr(*inputLine , " ")){
      char *token = strtok(*inputLine , " ");
      strcpy(cmd , token);
      token = strtok(NULL , " ");
      strcpy(dest , token);
      token = strtok(NULL , " ");
      while(token != NULL){
        strcat(dest , " ");
        strcat(dest , token);
        token = strtok(NULL , " ");
      }
    }
    if(!strcmp(cmd , "echo")){
      if(!strcmp(dest , "\n") || dest == NULL){printf(dest);return 1;} //false
      printf(dest); 
      return 1;
    }

    if(!strcmp(cmd , "exit")){
      printf("exit code: %i\n" , (u_int8_t)atoi(dest));
      return (u_int8_t)atoi(dest);
    }
    else{
      printf("bad command\n");
      return 1;
    }
}

void  getLine(char **inputLine){ //reading input 
    fgets(*inputLine , LEN_INPUT , stdin);
}

void shellMode(){
    int status = 1; //life cycle checker
    char *inputLine = malloc(sizeof(char) * LEN_INPUT);
    printf("Starting IC shell\n");
    modeCheck = 1;
  do{
      printf("icsh $ ");
      getLine(&inputLine);
      if(!strcmp(inputLine , "\n")){
        continue;
      }
      else{ status = commands(&inputLine);}
      
  } while(status == 1);
    
}

void scriptMode(){
  printf("script mode");
}

void main(int argc, char* argv[])
{
    if(argc > 1){
      char *dir = malloc(sizeof(char) * LEN_INPUT);
      scriptMode();
    }
    else{shellMode();}
}



 