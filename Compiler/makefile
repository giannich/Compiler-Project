CC=cc

all: lexer parser final

lexer: 
	lex lexer.lex

parser:
	yacc -d parser.y

final:
	$(CC) lex.yy.c y.tab.c -o aSmallCompiler

cleanCompilation:
	rm aSmallCompiler lex.yy.c y.tab.c y.tab.h

test:
	./aSmallCompiler < test.c

test2:
	$(CC) test.s -o test

cleanTest:
	rm test

clean:
	rm aSmallCompiler lex.yy.c y.tab.c y.tab.h test