%{
  #include <stdio.h> 
  #include "y.tab.h"
  #define CTRL_H 8
%}

%%
\003 { printf("CTRL+H detected\n"); }
"samarth"   {ECHO;printf(" IS COOL\n");}
"|"         {return PIPE;}
"("         {return OPEN_PARENTHESIS;}
")"         {return CLOSE_PARENTHESIS;}
"$"         {return DOLLAR;}
">"         {return GREAT;}
";"         {return SEMICOLON;}
"&"         {printf("& operator , command1 starts executing in background and command2 starts executing in foreground\n");}
&>          {printf("Redirect both stdout and stderr to a file\n");}
[ \t]       {printf("\n");}
[\n]        {return NEWLINE;}
(\".*\")    {printf("String literal: %s\n",yytext);}
(\'.*\')    {printf("String literal: %s\n",yytext);}
[-a-zA-Z\\/.~\$]+   {yylval.str = strdup(yytext);return WORD;}
%%

int yywrap(){
  return 0;
}

