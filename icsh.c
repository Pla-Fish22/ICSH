#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int modeCheck; //since script mode and shell mode will work differently
int LEN_INPUT = 1024;

int commands(char **command , char **dest){ //taking in commands 
    if(!strcmp(*command , "echo\n")){
      return 1;
    }
    if(!strcmp(*command , "echo ") || !strcmp(*command , "echo")){
        printf(*dest);
        return 1;
    }
    if(!strcmp(*command , "exit ")){
        if(modeCheck == 1){
          return (u_int8_t)(atoi(*dest));
        }
        else{
          printf("exit code: %i\n" , (u_int8_t)(atoi(*dest)) );
          return  0;
        }
    }
    if(!strcmp(*command , "0")){
      return 1;
    }
    printf("COMMAND NOT FOUND\n");
    return 1;
}

void  getLine(char **command , char **dest){ //reading input 
    char *input = malloc(sizeof(char) * LEN_INPUT);
    strcpy(*command , "0");
    fgets(input , LEN_INPUT , stdin);
    if((input[0] != '\n')){
      if(strchr(input , ' ') == NULL){
        strcpy(*command , input);
      }
      else{
        char *destTemp = strchr(input , ' ')+1;
        strcpy(*dest , destTemp);
        size_t lenOfCommand = destTemp - input;
        strncpy(*command , input , lenOfCommand);
      }
    }
    else{
      strcpy(*command , "0");
    }
    free(input);
}

void shellMode(){

    int status = 1; //life cycle checker
    char *prevCommand = malloc(sizeof(char) * LEN_INPUT);
    char *prevDest = malloc(sizeof(char) * LEN_INPUT);
    printf("Starting IC shell\n");
    modeCheck = 0;
  do{
      char *command = malloc(sizeof(char) * LEN_INPUT);
      char *dest = malloc(sizeof(char) * LEN_INPUT);
      printf("icsh $ ");
      getLine(&command , &dest);
      if(!strcmp(command , "!!\n")){ //check if command is !! if TRUE do the previous command instead
        status = commands(&prevCommand , &prevDest);
      }
      else{
        status = commands(&command , &dest);
        strcpy(prevCommand , command); strcpy(prevDest ,dest);
      }
      free(command);
      free(dest);
  } while(status == 1);
    free(prevCommand);
    free(prevDest);
    exit(EXIT_SUCCESS);
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



 