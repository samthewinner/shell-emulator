%{
 
 int yylex(void);
 void yyerror(char *);
 #include "shell.h"
 complex_command *cc;
 int cnt = 0;
 %} 

%union {int num;char* str;}
%token <num> NUMBER 
%token <str> WORD
%token <num> NEWLINE
%token <num> SEMICOLON
%token <num> PIPE 
%token <num> OPEN_PARENTHESIS
%token <num> CLOSE_PARENTHESIS
%token <num> DOLLAR 
%token <num> GREAT  

%%

cmd_list:cmd_list colon_list NEWLINE {/*fprintf(stderr,"Full command line ended , print output buffer and reset the stdin and stdout\n");*/ 
        execute(cc,STDOUT_FILENO,0); print_ob(cc); if(pipe(cc->ob)==-1)exit(1); prompt(cc);}
        |  
        ;

colon_list : colon_list SEMICOLON {execute(cc,STDOUT_FILENO,0);if(pipe(cc->ob)==-1)exit(1);} pipe_list
           | pipe_list
           ;

pipe_list: pipe_list PIPE {execute(cc,cc->ob[1],0);cc->prev_pipe = 1;} simple_command {cc->prev_pipe = 0;}   
         | simple_command 
         |
         ;

simple_command: cmd /*{fprintf(stderr,"command in io_mod_list , expected to get executed after taking all input files\n");} */ io_mod_list {
             /* fprintf(stderr,"IO command ended,execute IO command and store its Output in ob[2]\n");*/}
              ;

io_mod_list: io_mod_list GREAT WORD {
            cc->number_of_output_files++; 
            //open the file given by word,write empty string into the given file 
            int tmpfd = open($3,O_WRONLY|O_TRUNC|O_CREAT,0644);
            io_add_output_file(cc,$3);
            close(tmpfd);
           /* fprintf(stderr,"add file in the output list\n"); */
            }
           |
           ;


cmd: WORD {expand_command(cc,$1);} arg_list 
   ;

arg_list:arg_list WORD {expand_command(cc,$2);}
        |
        ;
%%

void yyerror(char *s) {
 fprintf(stderr, "Error: %s\n", s);
 restore(cc);
 if(pipe(cc->ob)==-1)
  exit(1);
 prompt(cc);
 yyparse();
} 

int main(){
/* execute(sc);prompt(); */

/*io_mod: 
       GREAT WORD {printf("Capture the input file for IO redirecion : %s\n ",$2);}
       ;
*/ 
cc = (complex_command*)malloc(sizeof(complex_command));
cc->simple_command = (simple_command*)malloc(sizeof(simple_command));
cc->outfile = (char**)malloc(sizeof(char)*100);


cc->og_output = dup(1);

if (pipe(cc->ob) == -1) {
               perror("pipe");
               exit(EXIT_FAILURE);
           }


  cc->simple_command = (simple_command*)malloc(sizeof(simple_command));
  cc->simple_command->curr_command_size = 0;
  cc->simple_command->max_command_size = 10;
  cc->simple_command->commands = (char**) malloc(cc->simple_command->max_command_size*sizeof(char*));
  cc->prev_pipe = 0;
  
  prompt(cc);
  signal(SIGINT, signal_handler);
  yyparse();
  return 1;
}
