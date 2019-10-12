# $Id: Makefile,v 1.8 2019-04-03 15:42:11-07 - - $

WARN   = -Wall -Wextra -Werror -Wpedantic -Wshadow -Wold-style-cast
CPP    = g++ -g -O0 -std=gnu++17 ${WARN} -fdiagnostics-color=never
CPPYY  = g++ -g -O0 -std=gnu++17 -Wno-register -fdiagnostics-color=never
OBJS   = main.o yyparse.o yylex.o
GENS   = yyparse.h yyparse.cpp yylex.cpp
SRCS   = extern.h lexer.l parser.y main.cpp Makefile

all : rpn

rpn : ${OBJS}
	${CPP} -o rpn ${OBJS}

%.o : %.cpp
	checksource $<
	cpplint.py.perl $<
	${CPP} -c $<

yyparse.cpp yyparse.h : parser.y
	bison --defines=yyparse.h --output=yyparse.cpp parser.y

yyparse.o : yyparse.cpp
	${CPPYY} -c yyparse.cpp

yylex.cpp : yyparse.h lexer.l
	flex --outfile=yylex.cpp lexer.l

yylex.o : yylex.cpp
	${CPPYY} -Wno-register -c yylex.cpp

spotless : clean
	- rm rpn Makefile.deps Listing.{ps,pdf}

clean :
	- rm ${OBJS} ${GENS} test.in test.out

ci : 
	- cid + ${SRCS}

test : rpn
	(echo "3 4 * 1 3 / +"; echo "10 0 /"; echo foo) \
	| tee test.in | rpn >test.out 2>&1
	more test.in test.out >test.listing </dev/null

lis : ${SRCS} test
	mkpspdf Listing.ps ${SRCS} yyparse.h test.listing

again : spotless
	${MAKE} ci all lis

Makefile.deps : yyparse.cpp yylex.cpp
	${CPP} -MM yyparse.cpp yylex.cpp main.cpp >Makefile.deps

include Makefile.deps

