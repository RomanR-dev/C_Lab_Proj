#include "utils/definitions.h"


/* struct macroTable {
    int set;
    char name[80];
    char cmnds[][80];
} macroTable;

void addMacro(struct macroTable * table, char * name, char ** cmd){
    int i = 0;
    while (table[i].set == 1){
        i++;
    }
    table[i].set = 1;
    strcpy(table[i].name, name);
    i = 0;
    while (sizeof(cmd) > i) {
        strcpy(table[i].cmnds[i], cmd[i]);
        i++;
    }
} */

void lstrip(char * l){
    int i = 0;
    int j = 0;
    while (l[i] == ' ') i++;
    while (l[i] != '\0'){
        l[j] = l[i];
        i++;
        j++;
    }
    l[j] = '\0';
}

char ** chooseParser(char * input){
    char * line;
    char ** parsedLine;
    int numOfOperands;
    line = strtok(input, "");
    numOfOperands = getOperandsCount(line);
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

int getOperandsCount(char * cmd){
    int numOfFuncs = sizeof(functions)/sizeof(functions[0]);
    int i;
    for (i=0; i<numOfFuncs; i++){
        if (strstr(cmd, functions[i].name)){
            return functions[i].operands;
        }
    }
    return -1;
}


char * concatenate(char ** toConCat, int max){
    char * line = malloc(80);
    switch (max){
        case 0:
            strcpy(line, toConCat[0]);
            break;
        case 1:
            strcpy(line, toConCat[0]);
            strcat(line, " ");
            strcat(line, toConCat[1]);
            break;
        case 2:
            strcpy(line, toConCat[0]);
            strcat(line, " ");
            strcat(line, toConCat[1]);
            strcat(line, ", ");
            strcat(line, toConCat[2]);
            break;
    }
    return line;
}

int main(int argc, char ** argv){
    char ** parsedLine;
    char line[80];
    int opCount;
    int i;
    FILE * inp = fopen("../tester/test1.txt", "r");
    FILE * outP = fopen("../tester/testOutP.txt", "w+");
    while ((fgets(line, 80, inp)) != NULL){ /* read from .as file and save macro data */
        if (strlen(line) > 80) {
            printf("line to long\n");
            continue;
        }
        parsedLine = chooseParser(line); /* parse line to get command and operands */
        i = 0;
        opCount = getOperandsCount(parsedLine[0]); /* check how many operands per command is defined */
        for (;i<=opCount;i++){
            printf("%s%s", parsedLine[i], i < opCount ? " ": "");
        }
        strcpy(line, concatenate(parsedLine, opCount));
        fputs(line, outP);
    }



/*    free(parsedLine); free(inp); free(outP); */
    return 0;
}