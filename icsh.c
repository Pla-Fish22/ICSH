#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>

#define LEN_INPUT 1024

typedef struct jobController{
  pid_t pid;
  int job_id;
  char command[LEN_INPUT];
  char state[LEN_INPUT];

};  

int quitStatus = 1;
int childExitCode;
pid_t fg;
int ccount;
int jobID;
pid_t bg;


int childHandler(int sig){
  int child_status;    
  pid_t pid;
   while ((pid = waitpid(-1, &child_status, WNOHANG)) > 0)
    {
        ccount--;
        if(pid == fg){return 0;}
        printf("Received signal %d from process %d\n",
               sig, pid);
    }
}


int buildInCommand(char cmd[], char *argv[], int max){
    int dup1Check = 0;
    int dup2Check = 0; 
    int bgCheck = 0;
    char file[1024];

    int idx;
    if(cmd[strlen(cmd) - 1] == '\n'){
      cmd[strlen(cmd) - 1] = '\0';
    }
    argv[0] = cmd;
    pid_t pid = fork();
    struct sigaction sig;
    printf(cmd);
    for(idx = 0; idx < max; idx++){
          printf(argv[idx]);
          // if(strchr(argv[idx] , '>')){
          //   argv[idx] = NULL;
          //   strcpy(file , argv[idx+1]);
          //   argv[idx+1] = NULL;
          //   dup1Check = 1;
          //   break;
          // }
      }

      // printf("%d" , dup1Check);
      // printf(file);



    if(pid < 0){printf("Error.\n");exit(EXIT_FAILURE);}

    if(pid == 0){
      sig.sa_handler = SIG_DFL;
      sigaction(SIGINT, &sig, NULL);
      sigaction(SIGTSTP, &sig, NULL);
      pid = getpid();
      setpgid(pid, pid);
      if(!bgCheck){
        tcsetpgrp(0, pid);
        fg = pid;
        printf("%d" , pid);
      }
      else{bg = pid;}
      jobID = ccount;
      if(dup1Check){
        int to = open(file, O_WRONLY | O_CREAT | O_TRUNC , 0777);
        if(file < 0){perror("failed to find file"); exit(EXIT_FAILURE);}
        int from = dup2(to , STDOUT_FILENO);
        close(file);
      }
      // if(dup2Check){
      //   int file = open(toFile, O_RDONLY);
      //   if(file < 0){perror("failed to find file"); exit(EXIT_FAILURE);}
      //   int from = dup2(file, STDIN_FILENO);
      //   close(file);
      // }
      execvp(cmd , argv);
      printf("bad command\n");
      exit(0);
    }

    else{
        int childStat;  
        setpgid(pid, pid);
        if(bgCheck){
            ccount++;
            bg = pid;
          }
          else{
             tcsetpgrp(0, pid);
             fg = pid;
             waitpid(pid , &childStat, WUNTRACED);
             tcsetpgrp(0, getpid());
             if(!WIFEXITED(childStat)){printf("\n");}
             if(WIFSIGNALED(childStat)){childExitCode = WTERMSIG(childStat) + 128;}
             if(WIFSTOPPED(childStat)){childExitCode = WSTOPSIG(childStat) + 128;}
          }
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
        if(!strcmp(token , "$?")){printf("%d\n" , childExitCode); return 0;}
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
      char *argv[10];
      char *token = strtok(temp2, " ");
      int idx = 0;
      int max;
      idx++;
      token = strtok(NULL , " ");
      while(token != NULL){
        if(token[strlen(token) - 1] == '\n'){token[strlen(token) - 1] = '\0';}
        argv[idx] = token;
        token = strtok(NULL , " ");
        idx++;
        max = idx;
      }

      int toRet = buildInCommand(temp2, argv , max);
      return toRet;

    }
        
}

void  getLine(char **inputLine){ //reading input 
    char temp[LEN_INPUT];
    fgets(temp , LEN_INPUT, stdin);
    strcpy(*inputLine , temp);
}

void shellMode(){

    struct sigaction sig, sigchld;

    sig.sa_handler = SIG_IGN; //go to handler to check which pid is running on back ground
    
    sigchld.sa_handler = childHandler;

    sigaction(SIGCHLD , &sigchld, NULL);

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



 
