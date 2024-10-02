all:  lexer parser bin

lexer: shell.l 
	flex shell.l 

bin: 
	gcc *.c -ll -o parser 

parser:
	yacc -d shell.y

clean: 
	@rm *.*.c *.*.h parser 

