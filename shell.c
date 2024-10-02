#include "shell.h"
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char *shell_builtin_commands[] = {"exit","cd","badal"};
const int builtin_count = 3;
char *prompt_string = NULL;

void signal_handler(int signum){
  return;
  // exit(0);
}

#include <stdio.h>
void red () {
  printf("\033[1;31m");
}

void yellow() {
  printf("\033[1;33m");
}

void reset () {
  printf("\033[0m");
}


void prompt(complex_command*cc){
  red();
  // dup2(cc->og_output,STDOUT_FILENO);
  if(!prompt_string)
  {
    char buf[100];
    getcwd(buf, sizeof(buf));
    printf("%s ",buf);
  }
  else  
    printf("%s ",prompt_string);
  // fprintf(stderr, "Prompt done\n");
  // dup2(cc->ob[1],STDOUT_FILENO);
  reset();
}

void expand_command(complex_command* cc,char *token){
  // fprintf(stderr, "string is : %s\n",token);
  // for(int i=0;i<cc->number_of_simple_commands;i++){
  //   fprintf(stderr, "Expand: %s\n",cc->simple_command->commands[i]);
  // }
  // simple_command *sc = cc->simple_command;
  // fprintf(stderr, "Expand command: %s\n",cc->simple_command->commands[0]);
  cc->simple_command->commands[cc->simple_command->curr_command_size] = strdup(token);
  // cc->simple_command->commands[cc->simple_command->curr_command_size+1] = NULL;
  cc->simple_command->curr_command_size++;
}

int is_builtin(char* cmd){
  for(int i=0;i<builtin_count;i++){
    if(strcmp(shell_builtin_commands[i], cmd)== 0)
      return i+1;
  }
  return 0;
}

void execute(complex_command* cc,int output_fd,int input_fd){
  if(!cc){
    fprintf(stderr, "%d %s \n",__LINE__,__FILE__);
    return;
  }
  // fprintf(stderr, "Execute started\n");
  simple_command *sc = cc->simple_command;
  if( !sc || !sc->commands || !sc->commands[0])
    {
    restore(cc);
    return ;
  }
  int ret = is_builtin(sc->commands[0]);
  
  if(ret){
    switch (ret) {
      case 1:
        exit(0);  
      case 2:
        // execvp(sc->commands[0],sc->commands);
        // printf("Yet to be handeled\n");
        if(sc->curr_command_size > 2){
          fprintf(stderr, "Wrong input given to cd\n");
          return ;
        }
        if(sc->curr_command_size == 1){
          chdir("/home/samarth");
        }
        else 
          chdir(sc->commands[1]);
        break;
      case 3:
        if(sc->commands[1] == NULL)
        {
          
          // fprintf(stderr, "Prompt can't be empty\n");
          // break;
        free(prompt_string);
          prompt_string = NULL;
        }
        else
          prompt_string = strdup(sc->commands[1]);
        
        break;
        
        
    };
    goto CMD_END;
  }
  
  int pid = fork();
  if(pid == -1){
    printf("Error\n");
    exit(0);
  }
  if(pid == 0){
  //child process
    // close(cc->ob[0])
    if(cc->prev_pipe == 1){
      // fprintf(stderr, "PREV PIPE IS ONE\n");
      dup2(cc->ob[0],STDIN_FILENO);
    }
    dup2(output_fd,STDOUT_FILENO);
    // dup2(input_fd,STDIN_FILENO);
    
    if(cc->number_of_output_files > 0)
    {
      int fd  = open(cc->outfile[cc->number_of_output_files-1],O_WRONLY|O_TRUNC|O_CREAT,0644);
      dup2(fd, STDOUT_FILENO);
    }
    // else{
    // 
    //   // dup2(cc->ob[1], STDOUT_FILENO);
    // }
      close(cc->ob[0]);
      close(cc->ob[1]);
      execvp(sc->commands[0],sc->commands);
    // fprintf(stderr, "Error\n");
    char err_msg[50];
    snprintf(err_msg, 40, "Cannot execute %s",sc->commands[0]);
    perror(err_msg);
    // perror("");
    // printf("execl returned! errno is [%d]\n",errno);
    // fprintf(stderr,"Error executing %s ",sc->commands[0]);
    // return ;
    exit(1);
  }
  close(cc->ob[1]);
  // close(cc->ob[0]);
  waitpid(pid,NULL,0);
  CMD_END:
  restore(cc);
    // fprintf(stderr, "Execute ended\n");
  
}

// void clean_command(simple_command* sc){
//   
//   for(int i=0;i<sc->curr_command_size;i++)
//     {
//       free(sc->commands[i]);
//       sc->commands[i] = NULL;
//     }
//   sc->curr_command_size = 0;
//   return;
// }

void restore(complex_command *cc){
  if(!cc){
    fprintf(stderr, "Empty cc in ob\n");
    return ;
  }
  // clean_command(cc->simple_command);
  
  for(int i=0;i<cc->number_of_output_files;i++){
    free(cc->outfile[i]);
    cc->outfile[i] = NULL;
  }
  for(int i=0;i<cc->simple_command->curr_command_size;i++)
    {
      free(cc->simple_command->commands[i]);
      cc->simple_command->commands[i] = NULL;
    }
  cc->simple_command->curr_command_size = 0;
  // cc->prev_pipe = 0;
//   
  // cc->simple_command = NULL;
  cc->number_of_output_files = 0;
  // if(pipe(cc->ob) == -1)
  // {
  //   fprintf(stderr, "Problem in pipe\n");
  //   exit(1);
  // }
    // fprintf(stderr, "Restore ended\n");
  }

void print_ob(complex_command *cc){
  if(!cc){
    fprintf(stderr, "Empty cc in ob\n");
    return ;
  }
  
  // fprintf(stderr, "print_ob stated\n");
  
  int pid = fork();
  if(pid == 0){

    char buffer[100];
    ssize_t bytes_read;
    // close(cc->ob[0]);
    dup2(cc->og_output,STDOUT_FILENO);
    close(cc->ob[1]);
    // close(cc->ob[1]);
    // int wfd = open(cc->outfile[cc->number_of_output_files-1],O_WRONLY | O_CREAT,0666);
    // close(cc->ob[1]);
    while ((bytes_read = read(cc->ob[0], buffer, sizeof(buffer))) > 0) {
      write(STDOUT_FILENO, buffer, bytes_read);
      // fprintf(stderr, "In\n");
    }
    close(cc->ob[0]);
    // close(cc->ob[0]);
    // close(cc->ob[1]);
    // fprintf(stderr, "print_ob ended\n");
    exit(0);
  }
    // dup2(cc->ob[1],1);
    close(cc->ob[1]);
    close(cc->ob[0]);
    waitpid(pid,NULL,0);
    // printf( "print_ob ended\n");
  
}

void io_add_output_file(complex_command* cc,char *file_name){
  if(!cc)return;
   cc->outfile[cc->number_of_output_files] = strdup(file_name);
   cc->number_of_output_files++;
}
