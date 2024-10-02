#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct simple_command{
  int curr_command_size;
  int max_command_size;
  char **commands;
}
simple_command;

typedef struct complex_command{
  
  simple_command * simple_command;
  int number_of_simple_commands;
  // int curr_simple_command;
  int number_of_output_files; 
  int ob[2];
  int og_output;
  int og_input;
  char **outfile;
  int prev_pipe;
  // char *input_file;
  // char *errfile;
  
}complex_command;

void execute(complex_command* cc,int fd,int input_fd);
void expand_command(complex_command* cc,char* token);
void prompt(complex_command *cc);
void signal_handler(int signum);
void clean_command(simple_command *sc);

void print_ob(complex_command *cc);

void io_add_output_file(complex_command* cc,char *file_name);
void restore(complex_command *cc);
