#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int LEN_INPUT = 1024;

int commands(char **command , char **dest){ //taking in commands 
    if(!strcmp(*command , "echo\n")){
      return 1;
    }
    if(!strcmp(*command , "echo ") || !strcmp(*command , "echo") ){
        printf(*dest);
        return 1;
    }
    if(!strcmp(*command , "exit") || !strcmp(*command , "exit\n")){
        printf("bye!");
        return 0;
    }
    if(!strcmp(*command , "0")){
      return 1;
    }
    printf("COMMAND NOT FOUND\n");
    return 1;
}

void  getLine(char **command , char **dest){ //reading input 
    char *input = malloc(sizeof(char) * LEN_INPUT);
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

void main(int argc, char* argv[])
{
    int status = 1; //life cycle checker
    char *command = malloc(sizeof(char) * LEN_INPUT);
    char *dest = malloc(sizeof(char) * LEN_INPUT);
    char *prevCommand = malloc(sizeof(char) * LEN_INPUT);
    char *prevDest = malloc(sizeof(char) * LEN_INPUT);
    printf("\nexe name=%s", argv[0]);
    for (int i=1; i< argc; i++) {
      printf("\narg%d=%s", i, argv[i]);
    }

    printf("Starting IC shell\n");
  do{
      printf("icsh $ ");
      getLine(&command , &dest);
      if(!strcmp(command , "!!\n")){ //check if command is !! if TRUE do the previous command instead
        status = commands(&prevCommand , &prevDest);
      }
      else{
        status = commands(&command , &dest);
        strcpy(prevCommand , command); strcpy(prevDest ,dest);
      }
  } while(status);
    free(command);
    free(prevCommand);
    free(dest);
    free(prevDest);
    exit(EXIT_SUCCESS);
}



 