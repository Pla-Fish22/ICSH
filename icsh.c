#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int modeCheck; //since script mode and shell mode will work differently
#define LEN_INPUT 9999

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
     printf("bye\n");
     return (u_int8_t)atoi(token);
    }
    if(!strcmp(temp , "!! ") || !strcmp(temp , "!!\n") || !strcmp(temp , "!!")){
     return commands(&prevInputLine, &inputLine);
    }
    else{
      printf("bad command\n");
      return 0;
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
    modeCheck = 1;
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
      
  } while(!status);
  free(inputLine);
  free(prevInputLine);
  exit(status);
    
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



 