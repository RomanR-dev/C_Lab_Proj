#ifndef _definitions_h
#define _definitions_h

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_LENGTH 80
#define MAX_COMMANDS 8192

typedef int bool;

enum {
    FALSE,
    TRUE
};

typedef struct {
    bool code;
    bool data;
    bool external;
    bool entry;
} attribute;

typedef struct symbol {
    char *name;
    int value;
    int baseAddress;
    int offset;
    attribute *attribs;

    bool isSet;
    bool hasNext;
    struct symbol *next;
} symbol;

typedef enum {
    /*binary*/
    sort0 = 0,  /*00 extra 1 word*/
    sort1 = 1, /*01 extra 2 words*/
    sort2 = 2,  /*10 extra 2 words*/
    sort3 = 3,  /*11 extra 0 words*/
    unsorted = 4
} sortType;


typedef struct {
    char *name;
    int opcode;
    int funct;
    int operands;
}func;

typedef struct {
    int set;
    char *name;
    char **cmnds;
    int numOfCmnds;
} macroTable;

typedef struct word1 {
    unsigned int destSort: 2;
    unsigned int destReg: 4;
    unsigned int sourceSort: 2;
    unsigned int sourceReg: 4;
    unsigned int funct: 4;
    unsigned int A: 1;
    unsigned int R: 1;
    unsigned int E: 1;
} word1;

typedef struct word2 {
    unsigned int opcode: 16;
    unsigned int A: 1;
    unsigned int R: 1;
    unsigned int E: 1;
} word2;

typedef struct machineCode {
    union word {
        word1 *code;
        word2 *data;
    } word;
    char set;
    char *declaredLabel;
    char *labelUsageSource;
    char *labelUsageDest;
    int L;
    bool additionalLine;
    bool isDataOrString;
} machineCode;

#endif
