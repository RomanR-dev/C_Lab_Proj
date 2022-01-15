CC = gcc
FLAGS = -ansi -Wall -pedantic

assembler: definitions.c tester/tester.c assembler.c
	${CC} -${FLAGS} -o assembler definitions.h definitions.c assembler.c

