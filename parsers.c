#include "definitions.h"
#include "utils.h"



/* parser funcs for asm code */
char ** parseWith2Operands(char * input) {
    char ** line = malloc(1);
    lstrip(input);
    line[0] = strtok(input, " ");
    line[1] = strtok(NULL, ",");
    line[2] = strtok(NULL, " ");
    return line;
}

char ** parseWith1Operand(char * input) {
    char ** line = malloc(1);
    lstrip(input);
    line[0] = strtok(input, " ");
    line[1] = strtok(NULL, ",");
    return line;
}

char ** parseNoOperands(char * input) {
    char ** line = malloc(1);
    lstrip(input);
    line[0] = strtok(input, "");
    return line;
}

int getOperandsCount(char * cmd, int * errors){
    int numOfFuncs;
    numOfFuncs = sizeof(functions)/sizeof(functions[0]);
    int i;
    for (i=0; i<numOfFuncs; i++){
        if (strstr(cmd, functions[i].name)){
            return functions[i].operands;
        }
    }
    *errors += 1;
    printf("--->Did not find command: %s\n", cmd);
    return -1;
}

char ** chooseParser(char * input, int * errors){
    char * line;
    char ** parsedLine;
    int numOfOperands;
    line = strtok(input, "");
    lstrip(line);
    numOfOperands = getOperandsCount(line, errors);
    if (numOfOperands == -1) return NULL;
    switch (numOfOperands){
        case 0:
            parsedLine = parseNoOperands(input);
            break;
        case 1:
            parsedLine = parseWith1Operand(input);
            break;
        case 2:
            parsedLine =  parseWith2Operands(input);
            break;
    }
    return parsedLine;
}