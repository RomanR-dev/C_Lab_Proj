#include "../misc/definitions.h"
#include "../misc/utils.h"
#include "../misc/parsers.h"
#include "pre_assembler.h"
#include "second_pass.h"

int codeDataOrString(char *line, machineCode *mCode, long *DC, bool withLabel, const long *IC) {
    int DCF = 0;
    /* get the string from between quotes, insert with for over len */
    char *tempLine = (char *) malloc(strlen(line) + 1);
    char *directive = (char *) malloc(strlen(line) + 1);
    int i = 0;
    long currNum;
    checkMalloc(tempLine);
    checkMalloc(directive);
    stringCopy(directive, line);
    stringCopy(tempLine, line);
    if (withLabel == TRUE) {
        strtok(directive, ":");
        directive = strtok(NULL, " ");
    } else {
        directive = strtok(directive, " ");
    }
    if (strstr(directive, ".string")) {
        if (withLabel == TRUE) {
            tempLine = strtok(tempLine, ":");
            mCode[*DC].declaredLabel = (char *) malloc(strlen(tempLine) + 1);
            mCode[*DC].set = 'd';
            checkMalloc(mCode[*DC].declaredLabel);
            stringCopy(mCode[*DC].declaredLabel, tempLine);
            stringCopy(tempLine, line);
        }
        strtok(tempLine, "”");
        tempLine = strtok(NULL, "");
        while (i < strlen(tempLine)) {
            if (isalnum(tempLine[i])) {
                mCode[*DC].word.data = malloc(sizeof(*mCode[*DC].word.data));
                checkMalloc(mCode[*DC].word.data);
                mCode[*DC].word.data->opcode = tempLine[i];
                mCode[*DC].set = 'd';
                setARE(*DC, mCode, 1, 0, 0);
                i++;
                *DC += 1;
                DCF++;
                continue;
            }
            i++;
        }
        mCode[*DC].word.data = malloc(sizeof(*mCode[*DC].word.data));
        checkMalloc(mCode[*DC].word.data);
        mCode[*DC].word.data->opcode = '\0';
        mCode[*DC].set = 'd';
        setARE(*DC, mCode, 1, 0, 0);
        *DC += 1;
        DCF++;
    } else {
        strtok(tempLine, ".data");
        if (withLabel == TRUE) {
            tempLine[strlen(tempLine) - 2] = '\0';
            mCode[*DC].declaredLabel = (char *) malloc(strlen(tempLine) + 1);
            checkMalloc(mCode[*DC].declaredLabel);
            stringCopy(mCode[*DC].declaredLabel, tempLine);
            mCode[*DC].set = 'd';
            tempLine = strtok(NULL, "");
            tempLine = strtok(tempLine, "data");
        } else {
            tempLine = strtok(tempLine, ".data");
        }
        if (tempLine[strlen(tempLine) - 1] == '\n') {
            tempLine[strlen(tempLine) - 1] = '\0';
        }
        if (strstr(tempLine, ",")) {
            tempLine = strtok(tempLine, ",");
        }
        while (tempLine != NULL) {
            mCode[*DC].word.data = malloc(sizeof(*mCode[*DC].word.data));
            mCode[*DC].set = 'd';
            currNum = strtol(tempLine, NULL, 10);
            mCode[*DC].word.data->opcode = currNum;
            setARE(*DC, mCode, 1, 0, 0);
            if (withLabel == FALSE) {
                mCode[*IC] = mCode[*DC];
                mCode[*DC].set = 'D';
            }
            *DC += 1;
            DCF++;
            tempLine = strtok(NULL, "");
        }
    }
    mCode[*DC - 1].L = DCF;
    return DCF;
}

bool labelAndDirectiveStep(char *line, symbol *head, long *IC, long *DC,
                           int *errors, machineCode *mCode, int *DCF, int *dataCounter) {
    bool isLabel = FALSE;
    bool isDirective = FALSE;
    symbol *currNode = head;
    symbol *tempNode = (symbol *) malloc(sizeof(symbol));
    attribute *attribs = (attribute *) malloc(sizeof(attribute));
    char *name = (char *) malloc(strlen(line) + 1);
    checkMalloc(name);
    stringCopy(name, line);
    checkMalloc(attribs);
    checkMalloc(tempNode);

    isLabel = checkIfLabel(line);
    isDirective = checkIfDirective(line);
    if (isLabel == TRUE && isDirective == TRUE) {
        *IC = *IC + *DCF;
        addSymbol(name, attribs, tempNode, head, currNode, *IC, line);
        *DCF = codeDataOrString(line, mCode, DC, isLabel, IC);
        *dataCounter += *DCF;
        return TRUE;
    } else if (isLabel == TRUE) {
        *IC = *IC + *DCF;
        addSymbol(name, attribs, tempNode, head, currNode, *IC, line);
        return FALSE;
    } else if (isDirective == TRUE) {
        *IC = *IC + *DCF;
        *DCF = codeDataOrString(line, mCode, DC, isLabel, IC);
        *dataCounter += *DCF;
        return TRUE;
    }
    return FALSE;
}

void externStep(char *line, symbol *head, int *errors, long IC) {
    size_t len;
    char *name = (char *) malloc(strlen(line) + 1);
    attribute *attribs = (attribute *) malloc(sizeof(attribute));
    symbol *currNode = head;
    symbol *tempNode = (symbol *) malloc(sizeof(symbol));
    checkMalloc(name);
    stringCopy(name, line);
    strtok(name, " ");
    name = strtok(NULL, "");
    len = strlen(name);
    if (name[len - 1] == ' ' || name[len - 1] == '\n') {
        name[len - 1] = '\0';
    }
    checkMalloc(attribs);

    checkMalloc(tempNode);
    addSymbol(name, attribs, tempNode, head, currNode, IC, line);
}

void alignTables(int ICF, symbol *head, machineCode *mCode) {
    symbol *tempNode = head;
    int i = 0;
    int tempCount;
    while (tempNode->isSet == TRUE) {
        if (tempNode->attribs->data == TRUE) {
            for (; i < ICF; i++) {
                if (mCode[i].set != '\0' && mCode[i].declaredLabel != NULL &&
                    strstr(tempNode->name, mCode[i].declaredLabel)) {
                    tempCount = tempNode->value;
                    mCode[tempCount] = mCode[i];
                    while (mCode[tempCount].L == 0) {
                        tempCount++;
                        i++;
                        mCode[tempCount] = mCode[i];
                    }
                }
                if (tempCount == ICF) {
                    break;
                }
            }
            i = 0;
        }
        if (tempNode->hasNext == TRUE) {
            tempNode = tempNode->next;
        } else {
            break;
        }
    }
}

bool firstPass(char *line, FILE *inp, int *errors, char *outPutFileName) {
    long IC = 100;
    long DC = 0;
    long zero = 0;
    int DCF = 0;
    bool is;
    char **parsedLine;
    char *tempLine;
    char *labelName = NULL;
    int dataCounter = 0;
    machineCode mCode[MAX_COMMANDS];
    symbol *head = malloc(sizeof(symbol));
    fseek(inp, 0, SEEK_SET);
    checkMalloc(head);

    stringCopy(line, iterator(line, inp, errors));
    errorHandler(errors, line);
    while (!(strstr(line, "NULL"))) {
        /*printf("checking line: %s\n", line);*/
        is = labelAndDirectiveStep(line, head, &IC, &DC, errors, mCode, &DCF, &dataCounter);
        if (is == TRUE) {
            stringCopy(line, iterator(line, inp, errors));
            errorHandler(errors, line);
            continue;
        }
        if (checkIfEntryOrExtern(line) == 2) { /*extern*/
            externStep(line, head, errors, zero);
            stringCopy(line, iterator(line, inp, errors));
            errorHandler(errors, line);
            continue;
        } else if (checkIfEntryOrExtern(line) == 1) {
            stringCopy(line, iterator(line, inp, errors));
            errorHandler(errors, line);
            continue;
        } /*entry - doing nothing first pass*/

        /*check if label + code*/
        tempLine = (char *) malloc(strlen(line) + 1);
        checkMalloc(tempLine);
        stringCopy(tempLine, line);
        if (checkIfLabel(line) == TRUE) {
            labelName = strtok(tempLine, ":");
            tempLine = strtok(NULL, "");
        }
        parsedLine = chooseParser(tempLine, errors);
        if (parsedLine == NULL) {
            break;
        }
        parseCmd(parsedLine, errors, tempLine, mCode, &IC, labelName);
        if (labelName != NULL) {
            free(labelName);
            labelName = NULL;
        }
        stringCopy(line, iterator(line, inp, errors));
        errorHandler(errors, line);
    }

    printf("\n===>>>>>> First pass finished with %d errors\n", *errors);
    free(labelName);
    alignTables(IC, head, mCode);
    if (*errors == 0) {
        secondPass(line, inp, errors, head, mCode, &IC, outPutFileName, &dataCounter);
    } else {
        fclose(inp);
        printf("due to errors not continuing with flow on current file, continue with next file...\n");
    }
    return TRUE;
}
