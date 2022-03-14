#include "definitions.h"
#include "utils.h"
#include "definitions.c"
#include "utils.h"


func functions[] = {
        {"mov",  0,  0,  2},
        {"cmp",  1,  0,  2},
        {"add",  2,  10, 2},
        {"sub",  2,  11, 2},
        {"lea",  4,  0,  2},

        {"clr",  5,  10, 1},
        {"not",  5,  11, 1},
        {"inc",  5,  12, 1},
        {"dec",  5,  13, 1},
        {"jmp",  9,  10, 1},
        {"bne",  9,  11, 1},
        {"jsr",  9,  12, 1},
        {"red",  12, 0,  1},
        {"prn",  13, 0,  1},

        {"rts",  14, 0,  0},
        {"stop", 15, 0,  0}
};

/* parser funcs for asm code */
char **parseWith2Operands(char *input) {
    bool hasDelim = FALSE;
    char **line = malloc(1);
    if (strstr(input, ",")) hasDelim = TRUE;
    lstrip(input);
    line[0] = strtok(input, " ");
    if (hasDelim == TRUE) {
        line[1] = strtok(NULL, ",");
    } else {
        line[1] = strtok(NULL, " ");
    }
    line[2] = strtok(NULL, " ");
    return line;
}

char **parseWith1Operand(char *input) {
    bool hasDelim = FALSE;
    char **line = malloc(1);
    if (strstr(input, ",")) hasDelim = TRUE;
    lstrip(input);
    line[0] = strtok(input, " ");
    if (hasDelim == TRUE) {
        line[1] = strtok(NULL, ",");
    } else {
        line[1] = strtok(NULL, " ");
    }
    line[2] = strtok(NULL, " ");
    return line;
}

char **parseNoOperands(char *input) {
    bool hasDelim = FALSE;
    char **line = malloc(1);
    if (strstr(input, ",")) hasDelim = TRUE;
    lstrip(input);
    line[0] = strtok(input, "");
    if (hasDelim == TRUE) {
        line[1] = strtok(NULL, ",");
    } else {
        line[1] = strtok(NULL, " ");
    }
    line[2] = strtok(NULL, " ");
    return line;
}

int getOperandsCount(char *cmd, int *errors) {
    int numOfFuncs;
    int i;
    char *cmdName = (char *) malloc(strlen(cmd));
    stringCopy(cmdName, cmd);
    cmdName = strtok(cmdName, " ");
    numOfFuncs = sizeof(functions) / sizeof(functions[0]);
    swapLastCharIfNewLine(cmdName);
    for (i = 0; i < numOfFuncs; i++) {
        if (strstr(cmdName, functions[i].name)) {
            return functions[i].operands;
        }
    }
    *errors += 1;
    printf("--->Error: CMD: [ %s ] not found.\n", cmdName);
    return -1;
}

bool assertNumOfOperands(char **parsedLine, int *errors, int numOfOperands) {
    switch (numOfOperands) {
        case 0:
            if (parsedLine[1] != NULL || parsedLine[2] != NULL) {
                *errors += 1;
                return FALSE;
            }
            break;
        case 1:
            if (parsedLine[1] == NULL || parsedLine[2] != NULL) {
                *errors += 1;
                return FALSE;
            }
            break;
        case 2:
            if (parsedLine[1] == NULL || parsedLine[2] == NULL) {
                *errors += 1;
                return FALSE;
            }
            break;
    }
    return TRUE;
}

char **chooseParser(char *input, int *errors) {
    char *line;
    char **parsedLine;
    int numOfOperands;
    line = strtok(input, "");
    if (line == NULL) return NULL;
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
    if (assertNumOfOperands(parsedLine, errors, numOfOperands) == FALSE) {
        swapLastCharIfNewLine(parsedLine[0]);
        printf("--->Wrong number of operands for command: %s\n", parsedLine[0]);
        return NULL;
    }
    if (strtok(NULL, " ") != NULL) {
        swapLastCharIfNewLine(parsedLine[0]);
        *errors += 1;
        printf("--->Wrong number of operands for command: %s\n", parsedLine[0]);
        return NULL;
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
        printf("--->Error: CMD: [ %s ] not found\n", cmd);
        *errors += 1;
    }
    setCode(mCode, IC, &functions[i], parsedLine, labelName, errors);
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
    if (isdigit(operand[0])) {
        return FALSE;
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

sortType getSortType(char *operand, int *errors) {
    if (strstr(operand, "\n") || strstr(operand, " ")) {
        operand[strlen(operand) - 1] = '\0';
    }
    /* if nothing, return invalid sort */
    if (operand[0] == '\0') return unsorted;
    /*immediate sort*/
    if (operand[0] == '#' && isInt(operand + 1)) return sort0;
    /*register sort*/
    if (operand[0] == 'r') {
        if ((atoi(&operand[1]) >= 0 && atoi(&operand[1]) <= 9 && operand[2] == '\0') ||
            (atoi(&operand[1]) >= 10 && atoi(&operand[1]) <= 15 && operand[3] == '\0')) {
            return sort3;
        }
    }
    /*direct sort*/
    if (isLabel(operand) == TRUE) return sort1;
    /*index sort*/
    if ((strstr(operand, "[") && strstr(operand, "]"))) return sort2;
        /*did not find appropriate sort*/
    else {
        *errors += 1;
        printf("--->Operand: %s, did not find matching sort type\n", operand);
        return unsorted;
    }
}

bool checkIfLabel(char *line) {
    char *tempLine = (char *) malloc(strlen(line) + 1);
    checkMalloc(tempLine);
    stringCopy(tempLine, line);
    swapLastCharIfNewLine(tempLine);
    tempLine = strtok(tempLine, " ");
    if (tempLine[strlen(tempLine) - 1] == ':') {
        return TRUE;
    }
    return FALSE;
}
