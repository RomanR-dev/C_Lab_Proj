#include "definitions.h"
#include "utils.h"
#include "parsers.h"

#include <unistd.h>

bool firstPass(char *line, FILE *inp, int *errors);

void setAdditionalLines(machineCode *mCode, long *IC, sortType sort, int *L, char *operand);

void setARE(int IC, machineCode *mCode, unsigned char A, unsigned char R, unsigned char E);

bool secondPass(char *line, FILE *outP, int *errors, symbol *head, machineCode *mCode, long *IC, long *DC);

void freeMachineCodes(machineCode *mCode);


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
    int foundMacro = 0;
    bool endm = FALSE;
    bool writeResult = FALSE;
    /* macro finder  */
    if (strstr(line, "macro")) {
        foundMacro = 1;
        char **tempLine = (char **) malloc(1);
        char *macroName = (char *) malloc(74);
        int counter = 0;
        strtok(line, " ");
        stringCopy(macroName, strtok(NULL, ":"));
        while ((fgets(line, 80, inp)) != NULL) { /* read from .as file and save macro data */
            if (strlen(line) > 80) {
                errors += 1;
                printf("line to long\n");
                continue;
            }
            if (strstr(line, "endm")) {
                endm = TRUE;
                table = addMacro(table, macroName, tempLine, counter);
                foundMacro = 0;
                break;
            }
            lstrip(line);
            tempLine[counter] = (char *) malloc(sizeof(char) * (strlen(line) + 1));
            stringCopy(tempLine[counter], line);
            counter++;
        }
        if (foundMacro == 1) {
            errors += 1;
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
    macroTable *table = malloc(sizeof(macroTable));
    FILE *outP;
    FILE *inp;

    while (inputFileCounter < argc) { /* iterate over files from argv .as */
        if ((inp = inputFileInit(argv, inp, &inputFileCounter)) == NULL) {
            perror("file not found, skipping to next");
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
            firstPass(line, outP, &errors);
        } else {
            fclose(outP);
            fclose(inp);
            perror("due to errors not continuing with flow on current file, continue with next file...");
        }
    }
    free(table);
//    free(outPutFileName);
    fclose(inp);
    fclose(outP);
}


bool checkIfLabel(char *line) {
    char *tempLine = (char *) malloc(strlen(line) + 1);
    stringCopy(tempLine, line);
    tempLine = strtok(tempLine, " ");
    if (tempLine[strlen(tempLine) - 1] == ':') {
        /* check whether there is code after */
        return TRUE;
    }
    return FALSE;
}

void codeDataOrString(char *line, machineCode *mCode, long *DC, bool withLabel) {
    /* get the string from between quotes, insert with for over len */
    char *tempLine = (char *) malloc(strlen(line) + 1);
    char *directive = (char *) malloc(strlen(line) + 1);
    int i = 0;
    int j = 0;
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
            stringCopy(mCode[*DC].declaredLabel, tempLine);
            stringCopy(tempLine, line);
        }
        tempLine = strtok(tempLine, "”");
        tempLine = strtok(NULL, "");
        while (i < strlen(tempLine)) {
            if (isalnum(tempLine[i])) {
                mCode[*DC].word.data = malloc(sizeof(*mCode[*DC].word.data));
                mCode[*DC].word.data->opcode = tempLine[i];
                mCode[*DC].set = 'd';
                setARE(*DC, mCode, 1, 0, 0);
                i++, j++;
                *DC += 1;
                continue;
            }
            i++;
        }
        mCode[*DC].word.data = malloc(sizeof(*mCode[*DC].word.data));
        mCode[*DC].word.data->opcode = '\0';
        mCode[*DC].set = 'd';
        setARE(*DC, mCode, 1, 0, 0);
        *DC += 1;
    } else {
        bool negative = FALSE;
        int insert;
        strtok(tempLine, ".data");
        if (withLabel == TRUE) {
            tempLine[strlen(tempLine) - 2] = '\0';
            mCode[*DC].declaredLabel = (char *) malloc(strlen(tempLine) + 1);
            stringCopy(mCode[*DC].declaredLabel, tempLine);
            tempLine = strtok(NULL, "");
            tempLine = strtok(tempLine, "data");
        }
        while (i < strlen(tempLine)) {
            if (isalnum(tempLine[i])) {
                if (negative == TRUE) {
                    insert = ((tempLine[i] - '0') * (-1));
                    negative = FALSE;
                } else {
                    insert = tempLine[i] - '0';
                }
                mCode[*DC].word.data = malloc(sizeof(*mCode[*DC].word.data));
                mCode[*DC].word.data->opcode = insert;
                mCode[*DC].set = 'd';
                setARE(*DC, mCode, 1, 0, 0);
                i++, j++;
                *DC += 1;
                continue;
            } else if (tempLine[i] == '-') {
                i++;
                negative = TRUE;
                continue;
            }
            i++;
        }
    }
}

bool labelAndDirectiveStep(char *line, symbol *head, long *IC, long *DC, int *errors, machineCode *mCode) {
    bool isLabel = FALSE;
    bool isDirective = FALSE;
    char *name = (char *) malloc(strlen(line) + 1);
    stringCopy(name, line);
    attribute *attribs = (attribute *) malloc(sizeof(attribute));

    symbol *currNode = head;
    symbol *tempNode = (symbol *) malloc(sizeof(symbol));

    isLabel = checkIfLabel(line);
    isDirective = checkIfDirective(line);
    if (isLabel == TRUE && isDirective == TRUE) {
        addSymbol(name, attribs, tempNode, head, currNode, IC, line);
        codeDataOrString(line, mCode, DC, isLabel);
        return TRUE;
    } else if (isLabel == TRUE) {
        addSymbol(name, attribs, tempNode, head, currNode, IC, line);
        return FALSE;
    } else if (isDirective == TRUE) {
        codeDataOrString(line, mCode, DC, isLabel);
        return TRUE;
    }
    return FALSE;
}

void externStep(char *line, symbol *head, int *errors, long *IC) {
    size_t len;
    char *name = (char *) malloc(strlen(line) + 1);
    stringCopy(name, line);
    strtok(name, " ");
    name = strtok(NULL, "");
    len = strlen(name);
    if (name[len - 1] == ' ' || name[len - 1] == '\n') {
        name[len - 1] = '\0';
    }
    attribute *attribs = (attribute *) malloc(sizeof(attribute));

    symbol *currNode = head;
    symbol *tempNode = (symbol *) malloc(sizeof(symbol));
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
    char *regName = (char *) malloc(3);
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

sortType getSortType(char *operand) {
    if (strstr(operand, "\n") || strstr(operand, " ")) {
        operand[strlen(operand) - 1] = '\0';
    }
    /* if nothing, return invalid sort */
    if (operand[0] == '\0') return unsorted;
    /*immediate sort*/
    if (operand[0] == '#' && isInt(operand + 1)) return sort0;
    /*register sort*/
    if (operand[0] == 'r' && ((atoi(&operand[1]) >= 0 && atoi(&operand[1]) <= 9 && operand[2] == '\0') ||
                              atoi(&operand[1]) >= 10 && atoi(&operand[1]) <= 15 && operand[3] == '\0'))
        return sort3;
    /*direct sort*/
    if (isLabel(operand) == TRUE) return sort1;
    /*index sort*/
    if ((strstr(operand, "[") && strstr(operand, "]"))) return sort2;
        /*did not find appropriate sort*/
    else return unsorted;
}

void setOperandLabel(sortType destSort, sortType sourceSort, char *labelName,
                     machineCode *mCode, char **parsedLine, long *IC, int operands) {
    if (destSort == sort1 && (labelName == NULL || mCode[*IC-1].declaredLabel != NULL)) {
        if (operands == 2){
            mCode[*IC - 1].labelUsage = (char *) malloc(strlen(parsedLine[2]) + 1);
            stringCopy(mCode[*IC - 1].labelUsage, parsedLine[2]);
        }
    } else if (sourceSort == sort1 && (labelName == NULL || mCode[*IC-1].declaredLabel != NULL)) {
        mCode[*IC - 1].labelUsage = (char *) malloc(strlen(parsedLine[1]) + 1);
        stringCopy(mCode[*IC - 1].labelUsage, parsedLine[1]);
    } else if (sourceSort == sort2 && (labelName == NULL || mCode[*IC-1].declaredLabel != NULL)) {
        mCode[*IC - 1].labelUsage = (char *) malloc(strlen(parsedLine[1]) + 1);
        stringCopy(mCode[*IC - 1].labelUsage, parsedLine[1]);
    } else if (destSort == sort2 && (labelName == NULL || mCode[*IC-1].declaredLabel != NULL)) {
        mCode[*IC - 1].labelUsage = (char *) malloc(strlen(parsedLine[1]) + 1);
        stringCopy(mCode[*IC - 1].labelUsage, parsedLine[1]);
    }
}

void setCode(machineCode *mCode, long *IC, func *f, char **parsedLine, char *labelName) {
    int L = 0;
    sortType sourceSort = unsorted;
    sortType destSort = unsorted;
    unsigned int destReg;
    unsigned int sourceReg;
    mCode[*IC].word.data = (word2 *) malloc(sizeof(*mCode[*IC].word.data));
    mCode[*IC].set = 'd';
    if (labelName != NULL) {
        mCode[*IC].declaredLabel = (char *) malloc(strlen(labelName) + 1);
        stringCopy(mCode[*IC].declaredLabel, labelName);
    }
    mCode[*IC].word.data->opcode = f->opcode;
    setARE(*IC, mCode, 1, 0, 0);

    L += 1;
    *IC += 1;
    mCode[*IC].word.code = (word1 *) malloc(sizeof(*mCode[*IC].word.code));
    mCode[*IC].set = 'c';
    mCode[*IC].word.code->funct = f->funct;
    if (f->operands == 1) {
        setARE(*IC, mCode, 1, 0, 0);
        mCode[*IC].word.code->sourceSort = 0;
        mCode[*IC].word.code->destReg = 0;
        mCode[*IC].word.code->sourceReg = 0;

        destSort = getSortType(parsedLine[1]);
        if (destSort == sort1 && labelName == NULL) {
            mCode[*IC - 1].declaredLabel = (char *) malloc(strlen(parsedLine[1]) + 1);
            stringCopy(mCode[*IC - 1].declaredLabel, parsedLine[1]);
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
        mCode[*IC].word.data = (word2 *) malloc(sizeof(*mCode[*IC].word.data));
        mCode[*IC].set = 'd';
        setARE(*IC, mCode, 1, 0, 0);
        num = strtol(++operand, NULL, 10);
        mCode[*IC].word.data->opcode = num;
        *L += 1;
        *IC += 1;
    } else if (sort == sort1 || sort == sort2) {
        mCode[*IC].word.data = (word2 *) malloc(sizeof(*mCode[*IC].word.data));
        mCode[*IC].set = 'd';
        setARE(*IC, mCode, 0, 0, 0);
        mCode[*IC].word.data->opcode = '?';/*dealt at 2nd pass*/
        *L += 1;
        *IC += 1;
        mCode[*IC].word.data = (word2 *) malloc(sizeof *(mCode[*IC].word.data));
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
    for (i = 0; i < 17; i++) {
        if (strstr(parsedLine[0], functions[i].name)) {
            found = TRUE;
            break;
        }
    }
    if (found == FALSE) *errors += 1;
    setCode(mCode, IC, &functions[i], parsedLine, labelName);
}

void errorHandler(int *errors, char *currLine) {
    char *lineForErrorHandling;
    lineForErrorHandling = (char *) malloc(strlen(currLine));
    stringCopy(lineForErrorHandling, currLine);
}

bool firstPass(char *line, FILE *inp, int *errors) {
    long IC = 100;
    long DC = 0;
    long zero = 0;
    int ICF, DCF;
    bool is = FALSE;
    char **parsedLine;
    char *tempLine;
    char *labelName = NULL;
    machineCode mCode[MAX_COMMANDS];
    fseek(inp, 0, SEEK_SET);
    symbol *head = malloc(sizeof(symbol));

    stringCopy(line, iterator(line, inp, errors));
    errorHandler(errors, line);
    while (!(strstr(line, "NULL"))) {
        printf("checking line: %s\n", line);
        is = labelAndDirectiveStep(line, head, &IC, &DC, errors, mCode);
        if (is == TRUE) {
            stringCopy(line, iterator(line, inp, errors));
            errorHandler(errors, line);
            continue;
        }
        if (checkIfEntryOrExtern(line) == 2) { /*extern*/
            externStep(line, head, errors, &zero);
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
    if (*errors == 0) {
        secondPass(line, inp, errors, head, mCode, &IC, &DC);
    } else {
        fclose(inp);
        perror("due to errors not continuing with flow on current file, continue with next file...\n");
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

bool secondPass(char *line, FILE *outP, int *errors, symbol *head, machineCode *mCode, long *IC, long *DC) {

    printf("second pass finished with %d errors", *errors);
    printf("\n============================================================\n");
    if (*errors == 0) {
//        createOutPutFIles();
    } else {
        perror("due to errors not continuing with flow on current file, continue with next file...\n");
    }
    return TRUE;
}


int main(int argc, char **argv) {
    mainRunner(argc, argv); /* first pass returns list of .am file names, updates amount in newArgc*/






//    free(newArgv);
    return 0;
}