#ifndef _definitions_h
#define _definitions_h

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#define TRUE true
#define FALSE false
#define MAX_COMMANDS 8192


typedef struct {
    char code;
    char data;
    char external;
    char entry;
} attribute;

typedef struct symbol{
    char * name;
    int value;
    int baseAddress;
    int offset;
    attribute * attribs;

    bool isSet;
    bool hasNext;
    struct symbol * next;

    void (*printPtr)(struct symbol * self);
} symbol;

typedef enum {
            /*binary*/
    sort0,  /*00 extra1 word*/
    sort1,  /*01 extra 2 words*/
    sort2,  /*10 extra 2 words*/
    sort3   /*11 extra 0 words*/
} sortType;


typedef struct {
    char * name;
    int opcode;
    int funct;
    int operands;
} func;

typedef struct {
    int set;
    char * name;
    char ** cmnds;
    int numOfCmnds;
} macroTable;


/* data consts */
static func functions[] = {
        {"mov", 0, -1, 2},
        {"cmp", 1, -1, 2},
        {"add", 2, 10, 2},
        {"sub", 2, 11, 2},
        {"lea", 4, -1, 2},

        {"clr", 5, 10, 1},
        {"not", 5, 11, 1},
        {"inc", 5, 12, 1},
        {"dec", 5, 13, 1},
        {"jmp", 9, 10, 1},
        {"bne", 9, 11, 1},
        {"jsr", 9, 12, 1},
        {"red", 12, -1, 1},
        {"prn", 13, -1, 1},

        {"rts", 14, -1, 0},
        {"stop", 15, -1, 0}
};




#endif