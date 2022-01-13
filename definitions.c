#include "definitions.h"
#include "parsers.h"
#include "utils.h"

void delay(int time){
    int i = 0;
    for (;i<time;i++);
}


macroTable * addMacro(macroTable * table, char * name, char ** cmd, int numOfCmds){
    int i = 0;
    int j = 0;
    while (table[j].set == 1){
        j++;
    }
    table = realloc(table, (sizeof(macroTable)*j) + sizeof(macroTable));
    table[j].set = 1;
    table[j].name = malloc(strlen(name));
    stringCopy(table[j].name, name);
    table[j].cmnds = malloc(1);
    while (numOfCmds > i) {
        table[j].cmnds[i] = malloc(strlen(cmd[i]));
        strcpy(table[j].cmnds[i], cmd[i]);
        i++;
    }
    table[j].numOfCmnds = numOfCmds;
    return table;
}

