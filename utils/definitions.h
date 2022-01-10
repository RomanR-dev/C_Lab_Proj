#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


struct func{
    char * name;
    int opcode;
    int funct;
    int operands;
};


struct func functions[] = {
        {"mov", 0, -1, 2},
        {"clr", 1, -1, 1},
        {"stop", 2, -1, 0}
};

int getOperandsCount(char * cmd);

char ** parseWith2Operands(char * input);

char ** parseWith1Operand(char * input);

char ** parseNoOperands(char * input);