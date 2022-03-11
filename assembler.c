#include "definitions.h"
#include "utils.h"
#include "parsers.h"

#include <unistd.h>

bool firstPass(char *line, FILE *inp, int *errors, char *outPutFileName);

void setAdditionalLines(machineCode *mCode, long *IC, sortType sort, int *L, char *operand);

void setARE(int IC, machineCode *mCode, unsigned char A, unsigned char R, unsigned char E);

bool secondPass(char *line, FILE *outP, int *errors, symbol *head, machineCode *mCode, long *IC, long *DC,
                char *outPutFileName);

void freeMachineCodes(machineCode *mCode);

bool checkMalloc(void *ptr) {
    if (ptr == NULL) {
        printf("malloc failed\n");
        return FALSE;
    }
    return TRUE;
}


bool macroWriter(char *line, macroTable *table, FILE *outP) {
    int macroCounter = 0;
    int i = 0;
    while (table[macroCounter].set == 1) {
        if (strstr(line, table[macroCounter].name) || strstr(table[macroCounter].name, line)) {
            /*write macro*/
            for (; i < table[macroCounter].numOfCmnds; i++) {
                fputs(table[macroCounter].cmnds[i], outP);
            }
            return TRUE;
        }
        macroCounter++;
    }
    return FALSE;
}

void preAssembler(char *line, int *errors, FILE *inp, FILE *outP, macroTable *table) {
    bool endm = FALSE;
    bool writeResult = FALSE;
    /* macro finder  */
    if (strstr(line, "macro")) {
        char **tempLine = (char **) malloc(1);
        char *macroName = (char *) malloc(74);
        checkMalloc(macroName);
        int counter = 0;
        strtok(line, " ");
        stringCopy(macroName, strtok(NULL, ":"));
        if (macroName[strlen(macroName) - 1] == '\n') macroName[strlen(macroName) - 1] = '\0';
        while ((fgets(line, 80, inp)) != NULL) { /* read from .as file and save macro data */
            if (*line == '\n') continue;
            if (strlen(line) > 80) {
                errors += 1;
                printf("line to long\n");
                continue;
            }
            if (strstr(line, "endm")) {
                endm = TRUE;
                table = addMacro(table, macroName, tempLine, counter);
                break;
            }
            lstrip(line);
            tempLine[counter] = (char *) malloc(strlen(line) + 1);
            checkMalloc(tempLine[counter]);
            stringCopy(tempLine[counter], line);
            counter++;
            if (strstr(line, macroName)) {
                *errors += 1;
                printError(
                        "macro doesnt have a closure with 'endm' will not create am file and continue with process.");
                break;
            }
        }
    }
    /* if written macro continue to next line, else write the line as is to new file */
    writeResult = macroWriter(line, table, outP);
    if (!writeResult && endm == false) {
        fprintf(outP, "%s", line);
    }
}

void mainRunner(int argc, char **argv) {
    int errors = 0;
    int inputFileCounter = 1;
    char line[MAX_LENGTH];
    char *outPutFileName = malloc(sizeof(char));
    checkMalloc(outPutFileName);
    macroTable *table = malloc(sizeof(macroTable));
    checkMalloc(table);
    FILE *outP;
    FILE *inp;

    while (inputFileCounter < argc) { /* iterate over files from argv .as */
        if ((inp = inputFileInit(argv, inp, &inputFileCounter)) == NULL) {
            printf("file not found, skipping to next");
            continue;
        } else outP = outputFileInit(outP, outPutFileName, argv[inputFileCounter - 1]);

        while (!(strstr(line, "NULL"))) {
            stringCopy(line, iterator(line, inp, &errors));
            if (strstr(line, "NULL")) break;
            preAssembler(line, &errors, inp, outP, table); /* macro handler */
        }
        printf("pre assembler finished: %s, errors: %d", argv[inputFileCounter - 1], errors);
        printf("\n============================================================\n");
        inputFileCounter++;
        if (errors == 0) {
            fclose(inp);
            firstPass(line, outP, &errors, outPutFileName);
        } else {
            fclose(outP);
            fclose(inp);
            printf("due to errors not continuing with flow on current file, continue with next file...");
        }
    }
    free(table);
    fclose(inp);
    fclose(outP);
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

int codeDataOrString(char *line, machineCode *mCode, long *DC, bool withLabel, long *IC) {
    int DCF = 0;
    /* get the string from between quotes, insert with for over len */
    char *tempLine = (char *) malloc(strlen(line) + 1);
    checkMalloc(tempLine);
    char *directive = (char *) malloc(strlen(line) + 1);
    checkMalloc(directive);
    int i = 0;
    long currNum = 0;
    stringCopy(directive, line);
    stringCopy(tempLine, line);
    if (withLabel == TRUE) {
        directive = strtok(directive, ":");
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
        tempLine = strtok(tempLine, "”");
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

bool labelAndDirectiveStep(char *line, symbol *head, long *IC, long *DC, int *errors, machineCode *mCode, int *DCF) {
    bool isLabel = FALSE;
    bool isDirective = FALSE;
    char *name = (char *) malloc(strlen(line) + 1);
    checkMalloc(name);
    stringCopy(name, line);
    attribute *attribs = (attribute *) malloc(sizeof(attribute));
    checkMalloc(attribs);
    symbol *currNode = head;
    symbol *tempNode = (symbol *) malloc(sizeof(symbol));
    checkMalloc(tempNode);

    isLabel = checkIfLabel(line);
    isDirective = checkIfDirective(line);
    if (isLabel == TRUE && isDirective == TRUE) {
        *IC = *IC + *DCF;
        addSymbol(name, attribs, tempNode, head, currNode, *IC, line);
        *DCF = codeDataOrString(line, mCode, DC, isLabel, IC);
        return TRUE;
    } else if (isLabel == TRUE) {
        *IC = *IC + *DCF;
        addSymbol(name, attribs, tempNode, head, currNode, *IC, line);
        return FALSE;
    } else if (isDirective == TRUE) {
        *IC = *IC + *DCF;
        *DCF = codeDataOrString(line, mCode, DC, isLabel, IC);
        return TRUE;
    }
    return FALSE;
}

void externStep(char *line, symbol *head, int *errors, long IC) {
    size_t len;
    char *name = (char *) malloc(strlen(line) + 1);
    checkMalloc(name);
    stringCopy(name, line);
    strtok(name, " ");
    name = strtok(NULL, "");
    len = strlen(name);
    if (name[len - 1] == ' ' || name[len - 1] == '\n') {
        name[len - 1] = '\0';
    }
    attribute *attribs = (attribute *) malloc(sizeof(attribute));
    checkMalloc(attribs);

    symbol *currNode = head;
    symbol *tempNode = (symbol *) malloc(sizeof(symbol));
    checkMalloc(tempNode);
    addSymbol(name, attribs, tempNode, head, currNode, IC, line);
}

int regNumber(char *reg, sortType sort) {
    if (sort == sort2) {
        reg = strtok(reg, "[");
        reg = strtok(NULL, ""); /* for regs in []*/
        reg[strlen(reg) - 1] = '\0';
    }
    ++reg;
    return atoi(reg);
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
                              atoi(&operand[1]) >= 10 && atoi(&operand[1]) <= 15 && operand[3] == '\0')) {
        return sort3;
    }
    /*direct sort*/
    if (isLabel(operand) == TRUE) return sort1;
    /*index sort*/
    if ((strstr(operand, "[") && strstr(operand, "]"))) return sort2;
        /*did not find appropriate sort*/
    else return unsorted;
}

void setOperandLabel(sortType destSort, sortType sourceSort, char *labelName,
                     machineCode *mCode, char **parsedLine, long *IC, int operands) {
    if (destSort == sort1 && (labelName == NULL || mCode[*IC - 1].declaredLabel != NULL)) {
        if (operands == 2) {
            mCode[*IC - 1].labelUsageDest = (char *) malloc(strlen(parsedLine[2]) + 1);
            checkMalloc(mCode[*IC - 1].labelUsageDest);
            stringCopy(mCode[*IC - 1].labelUsageDest, parsedLine[2]);
        }
    }
    if (sourceSort == sort1 && (labelName == NULL || mCode[*IC - 1].declaredLabel != NULL)) {
        mCode[*IC - 1].labelUsageSource = (char *) malloc(strlen(parsedLine[1]) + 1);
        checkMalloc(mCode[*IC - 1].labelUsageSource);
        stringCopy(mCode[*IC - 1].labelUsageSource, parsedLine[1]);
    }
    if (sourceSort == sort2 && (labelName == NULL || mCode[*IC - 1].declaredLabel != NULL)) {
        mCode[*IC - 1].labelUsageSource = (char *) malloc(strlen(parsedLine[1]) + 1);
        checkMalloc(mCode[*IC - 1].labelUsageSource);
        stringCopy(mCode[*IC - 1].labelUsageSource, parsedLine[1]);
    }
    if (destSort == sort2 && (labelName == NULL || mCode[*IC - 1].declaredLabel != NULL)) {
        mCode[*IC - 1].labelUsageDest = (char *) malloc(strlen(parsedLine[1]) + 1);
        checkMalloc(mCode[*IC - 1].labelUsageDest);
        stringCopy(mCode[*IC - 1].labelUsageDest, parsedLine[1]);
    }
}

void setCode(machineCode *mCode, long *IC, func *f, char **parsedLine, char *labelName) {
    int L = 0;
    sortType sourceSort = unsorted;
    sortType destSort = unsorted;
    unsigned int destReg;
    unsigned int sourceReg;
    mCode[*IC].word.data = (word2 *) malloc(sizeof(mCode[*IC].word));
    checkMalloc(mCode[*IC].word.data);
    mCode[*IC].set = 'd';
    if (labelName != NULL) {
        mCode[*IC].declaredLabel = (char *) malloc(strlen(labelName) + 1);
        checkMalloc(mCode[*IC].declaredLabel);
        stringCopy(mCode[*IC].declaredLabel, labelName);
    }
    mCode[*IC].word.data->opcode = f->opcode;
    setARE(*IC, mCode, 1, 0, 0);

    L += 1;
    *IC += 1;
    if (f->operands == 0) {
        mCode[*IC - 1].L = L;
        return;
    }
    mCode[*IC].word.code = (word1 *) malloc(sizeof(mCode[*IC].word));
    checkMalloc(mCode[*IC].word.code);
    mCode[*IC].set = 'c';
    mCode[*IC].word.code->funct = f->funct;
    if (f->operands == 1) {
        setARE(*IC, mCode, 1, 0, 0);
        mCode[*IC].word.code->sourceSort = 0;
        mCode[*IC].word.code->destReg = 0;
        mCode[*IC].word.code->sourceReg = 0;

        destSort = getSortType(parsedLine[1]);
        if (destSort == sort1 && labelName == NULL) {
            mCode[*IC - 1].labelUsageDest = (char *) malloc(strlen(parsedLine[1]) + 1);
            checkMalloc(mCode[*IC - 1].labelUsageDest);
            stringCopy(mCode[*IC - 1].labelUsageDest, parsedLine[1]);
        }
        mCode[*IC].word.code->destSort = destSort;

        if (destSort == sort3 || destSort == sort2) {
            destReg = regNumber(parsedLine[1], destSort);
            mCode[*IC].word.code->destReg = destReg;
        }
        setOperandLabel(destSort, sourceSort, labelName, mCode, parsedLine, IC, f->operands);
    } else if (f->operands == 2) {
        setARE(*IC, mCode, 1, 0, 0);
        mCode[*IC].word.code->destReg = 0;
        mCode[*IC].word.code->sourceReg = 0;

        sourceSort = getSortType(parsedLine[1]);
        destSort = getSortType(parsedLine[2]);
        mCode[*IC].word.code->sourceSort = sourceSort;
        mCode[*IC].word.code->destSort = destSort;

        if (sourceSort == sort3 || sourceSort == sort2) {
            sourceReg = regNumber(parsedLine[1], sourceSort);
            mCode[*IC].word.code->sourceReg = sourceReg;
        }
        if (destSort == sort3 || destSort == sort2) {
            destReg = regNumber(parsedLine[2], destSort);
            mCode[*IC].word.code->destReg = destReg;
        }
        setOperandLabel(destSort, sourceSort, labelName, mCode, parsedLine, IC, f->operands);
    }
    L += 1;
    *IC += 1;
    if (f->operands == 1) {
        setAdditionalLines(mCode, IC, destSort, &L, parsedLine[1]);
    } else if (f->operands == 2) {
        setAdditionalLines(mCode, IC, sourceSort, &L, parsedLine[1]);
        setAdditionalLines(mCode, IC, destSort, &L, parsedLine[2]);
    }
    mCode[*IC - 1].L = L;
}

void setARE(int IC, machineCode *mCode, unsigned char A, unsigned char R, unsigned char E) {
    mCode[IC].word.data->A = A;
    mCode[IC].word.data->R = R;
    mCode[IC].word.data->E = E;
}

void setAdditionalLines(machineCode *mCode, long *IC, sortType sort, int *L, char *operand) {
    long num;
    if (sort == sort0) {
        mCode[*IC].word.data = (word2 *) malloc(sizeof(mCode[*IC].word));
        checkMalloc(mCode[*IC].word.data);
        mCode[*IC].set = 'd';
        setARE(*IC, mCode, 1, 0, 0);
        num = strtol(++operand, NULL, 10);
        mCode[*IC].word.data->opcode = num;
        *L += 1;
        *IC += 1;
    } else if (sort == sort1 || sort == sort2) {
        mCode[*IC].word.data = (word2 *) malloc(sizeof(mCode[*IC].word));
        checkMalloc(mCode[*IC].word.data);
        mCode[*IC].set = 'd';
        setARE(*IC, mCode, 0, 0, 0);
        mCode[*IC].word.data->opcode = '?';/*dealt at 2nd pass*/
        *L += 1;
        *IC += 1;
        mCode[*IC].word.data = (word2 *) malloc(sizeof(mCode[*IC].word));
        checkMalloc(mCode[*IC].word.data);
        setARE(*IC, mCode, 0, 0, 0);
        mCode[*IC].set = 'd';
        mCode[*IC].word.data->opcode = '?';
        *L += 1;
        *IC += 1;
    }
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

void errorHandler(int *errors, char *currLine) {
    char *lineForErrorHandling;
    lineForErrorHandling = (char *) malloc(strlen(currLine) + 1);
    checkMalloc(lineForErrorHandling);
    stringCopy(lineForErrorHandling, currLine);
}

void alignTables(int ICF, symbol *head, machineCode *mCode) {
    symbol *tempNode = head;
    int i = 0;
    int tempCount;
    while (tempNode->hasNext == TRUE) {
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
        tempNode = tempNode->next;
    }
}

bool firstPass(char *line, FILE *inp, int *errors, char *outPutFileName) {
    long IC = 100;
    long DC = 0;
    long zero = 0;
    int i = 0;
    int ICF;
    int DCF = 0;
    bool is = FALSE;
    char **parsedLine;
    char *tempLine;
    char *labelName = NULL;
    machineCode mCode[MAX_COMMANDS];
    fseek(inp, 0, SEEK_SET);
    symbol *head = malloc(sizeof(symbol));
    symbol *tempNode = head;
    checkMalloc(head);

    stringCopy(line, iterator(line, inp, errors));
    errorHandler(errors, line);
    while (!(strstr(line, "NULL"))) {
        printf("checking line: %s\n", line);
        is = labelAndDirectiveStep(line, head, &IC, &DC, errors, mCode, &DCF);
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

    printf("first pass finished with %d errors", *errors);
    printf("\n============================================================\n");
    free(labelName);
    alignTables(IC, head, mCode);
    if (*errors == 0) {
        secondPass(line, inp, errors, head, mCode, &IC, &DC, outPutFileName);
    } else {
        fclose(inp);
        printf("due to errors not continuing with flow on current file, continue with next file...\n");
    }
    free(head);
    return TRUE;
}

void freeMachineCodes(machineCode *mCode) {
    int counter = 100;
    for (; counter < MAX_COMMANDS; counter++) {
        if (mCode[counter].set != '\0') {
            if (mCode[counter].set == 'd') free(mCode[counter].word.data);
            else if (mCode[counter].set == 'c') free(mCode[counter].word.code);
        }
        counter++;
    }
}

void entryStep(char *line, symbol *head) {
    symbol *temp;
    temp = head;
    char *tempLine = (char *) malloc(strlen(line) + 1);
    checkMalloc(tempLine);
    stringCopy(tempLine, line);
    strtok(tempLine, " ");
    tempLine = strtok(NULL, "");
    if (tempLine[strlen(tempLine) - 1] == ' ' || tempLine[strlen(tempLine) - 1] == '\n') {
        tempLine[strlen(tempLine) - 1] = '\0';
    }
    while (temp->hasNext == TRUE) {
        if (strstr(temp->name, tempLine)) {
            temp->attribs->entry = TRUE;
            break;
        }
        temp = temp->next;
    }
}

void fillBlanks(symbol *tempNode, machineCode *mCode, int i, char *labelUsage) {
    while (tempNode->hasNext) {
        if (labelUsage != NULL && strstr(labelUsage, tempNode->name)) {
            while (mCode[i].L == 0) {
                i++;
                if (mCode[i].word.data->opcode == 63) { /*check if ?????? in line*/
                    mCode[i].word.data->opcode = tempNode->baseAddress;
                    mCode[i + 1].word.data->opcode = tempNode->offset;
                    if (tempNode->attribs->external == TRUE) {
                        setARE(i, mCode, 0, 0, 1);
                        setARE(i + 1, mCode, 0, 0, 1);
                    } else {
                        setARE(i, mCode, 0, 1, 0);
                        setARE(i + 1, mCode, 0, 1, 0);
                    }
                }
            }
        }
        tempNode = tempNode->next;
    }
}

void createOutPutFileNames(char *orig, char *ext, char *ent, char *ob) {
    int len;
    stringCopy(ext, orig);
    stringCopy(ent, orig);
    stringCopy(ob, orig);
    len = strlen(ext);
    ext[len - 2] = 'e';
    ext[len - 1] = 'x';
    ext[len] = 't';
    ext[len + 1] = '\0';
    ent[len - 2] = 'e';
    ent[len - 1] = 'n';
    ent[len] = 't';
    ent[len + 1] = '\0';
    ob[len - 2] = 'o';
    ob[len - 1] = 'b';

}

void createEntryFile(char *fileName, symbol *head) {
    FILE *file = fopen(fileName, "w+");
    symbol *tempNode = head;
    while (tempNode->hasNext) {
        if (tempNode->attribs->entry == TRUE) {
            fprintf(file, "%s,%d,%d\n", tempNode->name, tempNode->baseAddress, tempNode->offset);
        }
        tempNode = tempNode->next;
    }
    fclose(file);
}

void createExternFile(char *fileName, symbol *head, machineCode *mCode) {
    FILE *file = fopen(fileName, "w+");
    symbol *tempNode = head;
    int i = 100;
    int base;
    int offset;
    while (tempNode->isSet) {
        if (tempNode->attribs->external == TRUE) {
            i = 100;
            while (mCode[i].set != '\0') {
                if (mCode[i].labelUsageDest != NULL && strstr(mCode[i].labelUsageDest, tempNode->name)) {
                    while (mCode[i].L == 0) {
                        i++;
                    }
                    base = i - 1;
                    offset = base + 1;
                }
                if (mCode[i].labelUsageSource != NULL && strstr(mCode[i].labelUsageSource, tempNode->name)) {
                    base = i + 2;
                    offset = base + 1;
                }
                i++;
            }
            fprintf(file, "%s BASE %d\n%s OFFSET %d\n\n", tempNode->name, base, tempNode->name, offset);
        }
        if (tempNode->hasNext) {
            tempNode = tempNode->next;
        } else {
            break;
        }
    }
    fclose(file);
}

bool createOutPutFiles(machineCode *mCode, symbol *head, char *outPutFileName) {
    char *extFileName = (char *) malloc(strlen(outPutFileName) + 1);
    char *entFileName = (char *) malloc(strlen(outPutFileName) + 1);
    char *obFileName = (char *) malloc(strlen(outPutFileName));
    int i;

    createOutPutFileNames(outPutFileName, extFileName, entFileName, obFileName);

    createEntryFile(entFileName, head);
    createExternFile(extFileName, head, mCode);

    return TRUE;
}

bool secondPass(char *line, FILE *inp, int *errors, symbol *head, machineCode *mCode, long *IC, long *DC,
                char *outPutFileName) {
    int i = 0;
    symbol *tempNode = head;
    fseek(inp, 0, SEEK_SET);

    stringCopy(line, iterator(line, inp, errors));
    while (!(strstr(line, "NULL"))) {
        if (checkIfEntryOrExtern(line) == 1) {
            entryStep(line, head);
            stringCopy(line, iterator(line, inp, errors));
            continue;
        }
        stringCopy(line, iterator(line, inp, errors));
    }
    for (; i < *IC; i++) {
        if (mCode[i].labelUsageSource != NULL) {
            fillBlanks(tempNode, mCode, i, mCode[i].labelUsageSource);
            tempNode = head;
        }
        if (mCode[i].labelUsageDest != NULL) {
            fillBlanks(tempNode, mCode, i, mCode[i].labelUsageDest);
            tempNode = head;
        }
    }

    printf("second pass finished with %d errors", *errors);
    printf("\n============================================================\n");
    if (*errors == 0) {
        fclose(inp);
        createOutPutFiles(mCode, head, outPutFileName);
    } else {
        fclose(inp);
        printError("due to errors not continuing with flow on current file, continue with next file...\n");
    }
    return TRUE;
}


int main(int argc, char **argv) {
    mainRunner(argc, argv); /* first pass returns list of .am file names, updates amount in newArgc*/






//    free(newArgv);
    return 0;
}