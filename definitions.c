#include "definitions.h"
#include "parsers.h"
#include "utils.h"



void addMacro(macroTable * table, char * name, char ** cmd, int numOfCmds){
    int i = 0;
    int j = 0;
    while (table[j].set == 1){
        j++;
    }
    table[j].set = 1;
    table[j].name = malloc(strlen(name));
    strcpy(table[i].name, name);
    table[j].cmnds = malloc(1);
    while (numOfCmds > i) {
        table[j].cmnds[i] = malloc(strlen(cmd[i]));
        strcpy(table[j].cmnds[i], cmd[i]);
        i++;
    }
}

