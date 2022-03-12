#include "definitions.h"
#include "utils.h"
#include "definitions.c"


/* parser funcs for asm code */
char **parseWith2Operands(char *input) {
    char **line = malloc(1);
    lstrip(input);
    line[0] = strtok(input, " ");
    line[1] = strtok(NULL, ",");
    line[2] = strtok(NULL, " ");
    return line;
}

char **parseWith1Operand(char *input) {
    char **line = malloc(1);
    lstrip(input);
    line[0] = strtok(input, " ");
    line[1] = strtok(NULL, ",");
    return line;
}

char **parseNoOperands(char *input) {
    char **line = malloc(1);
    lstrip(input);
    line[0] = strtok(input, "");
    return line;
}

int getOperandsCount(char *cmd, int *errors) {
    int numOfFuncs;
    int i;
    numOfFuncs = sizeof(functions) / sizeof(functions[0]);
    for (i = 0; i < numOfFuncs; i++) {
        if (strstr(cmd, functions[i].name)) {
            return functions[i].operands;
        }
    }
    *errors += 1;
    printf("--->Did not find command: %s\n", cmd);
    return -1;
}

char **chooseParser(char *input, int *errors) {
    char *line;
    char **parsedLine;
    int numOfOperands;
    line = strtok(input, "");
    lstrip(line);
    numOfOperands = getOperandsCount(line, errors);
    if (numOfOperands == -1) return NULL;
    switch (numOfOperands) {
        case 0:
            parsedLine = parseNoOperands(input);
            break;
        case 1:
            parsedLine = parseWith1Operand(input);
            break;
        case 2:
            parsedLine = parseWith2Operands(input);
            break;
    }
    return parsedLine;
}

void parseCmd(char **parsedLine, int *errors, char *cmd, machineCode *mCode, long *IC, char *labelName) {
    bool found = FALSE;
    int i;
    if (cmd[strlen(cmd) - 1] == '\n') {
        cmd[strlen(cmd) - 1] = '\0';
    }
    for (i = 0; i < 17; i++) {
        if (strstr(parsedLine[0], functions[i].name) && strstr(functions[i].name, parsedLine[0])) {
            found = TRUE;
            break;
        }
    }
    if (found == FALSE) {
        printf("Error: CMD: [ %s ] not found\n", cmd);
        *errors += 1;
    }
    setCode(mCode, IC, &functions[i], parsedLine, labelName);
}

bool isInt(char *num) {
    int i = 0;
    if (num[0] == '-' || num[0] == '+') num++;
    if (num[strlen(num) - 1] == '\n') {
        num[strlen(num) - 1] = '\0';
    }
    for (; num[i]; i++) {
        if (!isdigit(num[i])) {
            return FALSE;
        }
    }
    return i > 0;
}

bool isValidLabelName(char *label) {
    int i = 0;
    char *regName = (char *) malloc(4);
    checkMalloc(regName);
    for (; i < 16; i++) {
        if (strstr(label, functions[i].name)) {
            return FALSE;
        }
    }
    i = 0;
    for (; i < 16; i++) {
        sprintf(regName, "r%d", i);
        if (strstr(regName, label)) {
            return FALSE;
        }
    }
    return TRUE;
}

bool isLabel(char *operand) {
    int len;
    int count = 0;
    len = strlen(operand);
    if (operand[len - 1] == '\n') {
        operand[len - 1] = '\0';
        count = 1;
    }
    if (isValidLabelName(operand) == TRUE) {
        if ((isalpha(*operand) && *operand != '\0')) {
            len--;
            operand++;
            while (len > count) {
                if (!(isalnum(*operand) && *operand != '\0')) {
                    return FALSE;
                }
                len--;
                operand++;
            }
        }
    }
    return TRUE;
}

sortType getSortType(char *operand) { /*TODO check why sporadic failures right hereeeeeeeee*/
    if (strstr(operand, "\n") || strstr(operand, " ")) {
        operand[strlen(operand) - 1] = '\0';
    }
    /* if nothing, return invalid sort */
    if (operand[0] == '\0') return unsorted;
    /*immediate sort*/
    if (operand[0] == '#' && isInt(operand + 1)) return sort0;
    /*register sort*/
    if (operand[0] == 'r' && ((atoi(&operand[1]) >= 0 && atoi(&operand[1]) <= 9 && operand[2] == '\0') ||
                              (atoi(&operand[1]) >= 10 && atoi(&operand[1]) <= 15 && operand[3] == '\0'))) {
        return sort3;
    }
    /*direct sort*/
    if (isLabel(operand) == TRUE) return sort1;
    /*index sort*/
    if ((strstr(operand, "[") && strstr(operand, "]"))) return sort2;
        /*did not find appropriate sort*/
    else return unsorted;
}

bool checkIfLabel(char *line) {
    char *tempLine = (char *) malloc(strlen(line) + 1);
    checkMalloc(tempLine);
    stringCopy(tempLine, line);
    tempLine = strtok(tempLine, " ");
    if (tempLine[strlen(tempLine) - 1] == ':') {
        /* check whether there is code after */
        return TRUE;
    }
    return FALSE;
}
