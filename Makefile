CC = gcc
FLAGS = -ansi -Wall -pedantic

assembler: definitions.c tester/tester.c assembler.c
	${CC} -${FLAGS} -o assembler utils/definitions.h utils/definitions.c tester/tester.h tester/tester.c assembler.c

