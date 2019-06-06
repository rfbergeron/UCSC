#Makefile
#Author: Robert Bergeron
#ID: rbergero
#Makefile for Lab4, with macros

FLAGS = -std=c99 -Wall
SOURCES = balanced.c
OBJECTS = balanced.o
EXEBIN = balanced

all: $(EXEBIN)

$(EXEBIN) : $(OBJECTS)
	gcc -o $(EXEBIN) $(OBJECTS)

$(OBJECTS) : $(SOURCES)
	gcc -c $(FLAGS) $(SOURCES)

clean :
	rm -f $(EXEBIN) $(OBJECTS)
