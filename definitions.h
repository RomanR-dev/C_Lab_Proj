#ifndef _definitions_h
#define _definitions_h

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>



typedef struct f {
    char * name;
    int opcode;
    int funct;
    int operands;
} func;


typedef struct m {
    int set;
    char *name;
    char ** cmnds;
} macroTable;

void addMacro(macroTable * table, char * name, char ** cmd, int numOfCmds);


/* data consts */
static func functions[] = {
        {"mov", 0, -1, 2},

        {"cmp", 1, -1, 1},

        {"add", 2, 10, 0},
        {"sub", 2, 11, 0},

        {"lea", 4, -1, 0},

        {"clr", 5, 10, 0},
        {"not", 5, 11, 0},
        {"inc", 5, 12, 0},
        {"dec", 5, 13, 0},

        {"jmp", 9, 10, 0},
        {"bne", 9, 11, 0},
        {"jsr", 9, 12, 0},

        {"red", 12, -1, 0},

        {"prn", 13, -1, 0},

        {"rts", 14, -1, 0},

        {"stop", 15, -1, 0}
};




#endif