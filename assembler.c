#include "definitions.h"
#include "utils.h"
#include "parsers.h"

bool firstPass(char *line, FILE *inp, int *errors);


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
            strcpy(line, iterator(line, inp, &errors));
            if (strstr(line, "NULL")) break;
            preAssembler(line, &errors, inp, outP, table); /* macro handler */
        }
        printf("pre assembler finished: %s, errors: %d\n", argv[inputFileCounter - 1], errors);
        inputFileCounter++;
        if (errors == 0) {
            fclose(inp);
            firstPass(line, outP, &errors);
        } else {
            fclose(outP);
            fclose(inp);
            perror("due to errors not continuing with flow on current file, continue with next file...");
        }
        printf("============================================================\n");
    }
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

void codeDataOrString(char *line, machineCode *mCode, int *DC, bool withLabel) {
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
        tempLine = strtok(tempLine, "”");
        tempLine = strtok(NULL, "");
        while (i < strlen(tempLine)) {
            if (isalnum(tempLine[i])) {
                mCode[*DC].word.data = malloc(sizeof(*mCode[*DC].word.data));
                mCode[*DC].word.data->opcode = tempLine[j];
                mCode[*DC].word.data->A = 1;
                i++, j++;
                *DC += 1;
                continue;
            }
            i++;
        }
        mCode[*DC].word.data = malloc(sizeof(*mCode[*DC].word.data));
        mCode[*DC].word.data->opcode = '\0';
        mCode[*DC].word.data->A = 1;
        *DC += 1;
    } else {
        bool negative = FALSE;
        unsigned int insert;
        strtok(tempLine, ".data");
        tempLine = strtok(NULL, "");
        tempLine = strtok(tempLine, "data");
        while (i < strlen(tempLine)) {
            if (isalnum(tempLine[i])) {
                if (negative == TRUE) {
                    insert = tempLine[j] * (-1);
                    negative = FALSE;
                } else {
                    insert = tempLine[j];
                }
                mCode[*DC].word.data = malloc(sizeof(*mCode[*DC].word.data));
                mCode[*DC].word.data->opcode = insert;
                mCode[*DC].word.data->A = 1;
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

bool labelAndDirectiveStep(char *line, symbol *head, int *IC, int *DC, int *errors, machineCode *mCode) {
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
    }
    return FALSE;
}

void externStep(char *line, symbol *head, int *errors, int *IC) {
    size_t len;
    char *name = (char *) malloc(strlen(line) + 1);
    stringCopy(name, line);
    strtok(name, " ");
    name = strtok(NULL, "");
    len = strlen(name);
    name[len - 1] = '\0';
    attribute *attribs = (attribute *) malloc(sizeof(attribute));

    symbol *currNode = head;
    symbol *tempNode = (symbol *) malloc(sizeof(symbol));
    addSymbol(name, attribs, tempNode, head, currNode, IC, line);
}

int regNumber(char *reg) {
    return atoi(++reg);
}

bool isInt(char *string) {
    int i = 0;
    if (string[0] == '-' || string[0] == '+') string++;
    for (; string[i]; i++) {
        if (!isdigit(string[i])) {
            return FALSE;
        }
    }
    return i > 0;
}

bool isLabel(char *operand) {
    int len;
    len = strlen(operand);
    while (len > 0){
        if (!(isalpha(*operand))){
            return FALSE;
        }
        len--;
        operand++;
    }
    return TRUE;
}

sortType getSortType(char *operand) {
    /* if nothing, return invalid sort */
    if (operand[0] == '\0') return unsorted;
    /*immediate sort*/
    if (operand[0] == '#' && isInt(operand + 1)) return sort0;
    /*direct sort*/
    if (isLabel(operand) == TRUE) return sort1;
    /*index sort*/
    if ((strstr(operand, "[") && strstr(operand, "]"))) return sort2;
    /*register sort*/
    if (operand[0] == 'r' && atoi(&operand[1]) >= 0 && atoi(&operand[1]) <= 15 && operand[2] == '\0') return sort3;

    else return unsorted;
}

void setCode(machineCode *mCode, int *IC, func *f, int *L, char **parsedLine) {
    sortType sType1;
    sortType sType2;
    mCode[*IC].word.data->opcode = f->opcode;
    mCode[*IC].word.data->A = 1;
    *L += 1;
    mCode[*IC].word.code->funct = f->funct;

    if (f->operands == 1) {
        sType1 = getSortType(parsedLine[1]);
    } else if (f->operands == 2) {
        sType1 = getSortType(parsedLine[1]);
        sType2 = getSortType(parsedLine[2]);
    }
}

void parseCmd(char **parsedLine, int *errors, char *cmd, int *L, machineCode *mCode, int *IC) {
    int operands;
    bool found = FALSE;
    int i;
    for (i = 0; i < 17; i++) {
        if (strstr(parsedLine[0], functions[i].name)) {
            found = TRUE;
            operands = functions[i].operands;
            break;
        }
    }
    if (found == FALSE) *errors += 1;
}

bool firstPass(char *line, FILE *inp, int *errors) {
    int IC = 100;
    int DC = 0;
    int zero = 0;
    int L;
    int ICF, DCF;
    bool is = FALSE;
    char **parsedLine;
    char *tempLine;
    machineCode mCode[MAX_COMMANDS];
    fseek(inp, 0, SEEK_SET);
    symbol *head = malloc(sizeof(symbol));

    strcpy(line, iterator(line, inp, errors));
    while (!(strstr(line, "NULL"))) {
        printf("checking line: %s\n", line);
        is = labelAndDirectiveStep(line, head, &IC, &DC, errors, mCode);
        if (is == TRUE) {
            strcpy(line, iterator(line, inp, errors));
            continue;
        }
        if (checkIfEntryOrExtern(line) == 2) { /*extern*/
            externStep(line, head, errors, &zero);
            strcpy(line, iterator(line, inp, errors));
            continue;
        } else if (checkIfEntryOrExtern(line) == 1) {
            strcpy(line, iterator(line, inp, errors));
            continue;
        } /*entry - doing nothing first pass*/

        /*check if label + code*/
        tempLine = (char *) malloc(strlen(line) + 1);
        stringCopy(tempLine, line);
        if (checkIfLabel(line) == TRUE) {
            strtok(tempLine, ":");
            tempLine = strtok(NULL, "");
        }
        parsedLine = chooseParser(tempLine, errors);
        parseCmd(parsedLine, errors, tempLine, &L, mCode, &IC);

        strcpy(line, iterator(line, inp, errors));
    }

    /* check if label in said line */
    printf("first pass finished with %d errors", *errors);
    if (*errors == 0) {
        fclose(inp);
        /*secondPass(line, outP, &errors);*/
    } else {
        fclose(inp);
        perror("due to errors not continuing with flow on current file, continue with next file...");
    }
    printf("\n============================================================\n");
    return true;
}


char **secondPass() {
    char **parsedLine;


    return parsedLine;
}


int main(int argc, char **argv) {
    mainRunner(argc, argv); /* first pass returns list of .am file names, updates amount in newArgc*/




//    free(newArgv);
    return 0;
}