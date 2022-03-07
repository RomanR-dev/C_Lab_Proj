#include "definitions.h"
#include "utils.h"
#include "parsers.h"

bool firstPass(char *line, FILE *inp, int *errors);

char *concatenate(char **toConCat, int max) {
    char *line = malloc(80);
    switch (max) {
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


void printWrittenLine(int opCount, char **parsedLine) {
    int i = 0;
    for (; i <= opCount; i++) {
        printf("%s%s", parsedLine[i], i < opCount ? " " : "");
    }
}


bool lineSplitterFuncAndConcatenate(char **parsedLine, int opCount, char *line, int errors) {
    parsedLine = chooseParser(line, &errors); /* parse line to get command and operands */
    if (!parsedLine) return FALSE;
    opCount = getOperandsCount(parsedLine[0], &errors); /* check how many operands per command is defined */
    if (opCount == -1) return FALSE;

    printWrittenLine(opCount, parsedLine); /* for debug purposes */
    strcpy(line, concatenate(parsedLine, opCount)); /* concat split line and copy it to the line str */
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
            tempLine[counter] = (char *) malloc(sizeof(char) * (strlen(line)+1));
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

char *iterator(char *line, FILE *inp, int *errors) {
    while ((fgets(line, MAX_LENGTH, inp)) != NULL) { /* read from .as file and save macro data to .am file */
        if (strlen(line) > MAX_LENGTH) {
            errors += 1;
            printf("line to long\n");
            continue;
        }
        if (line[0] == '\n') continue;
        if (line[0] == ';') continue;
        lstrip(line);
        return line;
    }
    return "NULL";
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


int checkIfAttrib(char *attrib) {
    if (strstr(".entry", attrib) || strstr(".extern", attrib)) {
        return 1;
    } else if (strstr(".data", attrib) || strstr(".string", attrib)) {
        return 2;
    }
    return 0;
}

bool checkIfDirective(char *line) {
    int res;
    char *tempLine = (char *) malloc(strlen(line)+1);
    stringCopy(tempLine, line);
    strtok(tempLine, " ");
    tempLine = strtok(NULL, " ");
    if (tempLine != NULL && tempLine[0] == '.') {
        res = checkIfAttrib(tempLine); /* check if attrib*/
        if (res == 2) {
            return TRUE;
        }
    }
    return FALSE;
}

bool checkIfLabel(char *line) {
    char *tempLine = (char *) malloc(strlen(line)+1);
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
    char *tempLine = (char *) malloc(strlen(line)+1);
    char *directive = (char *) malloc(strlen(line)+1);
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

    mCode[*DC].word.data = malloc(sizeof(*mCode[*DC].word.data));
    mCode[*DC].word.data->opcode = '\0';
    mCode[*DC].word.data->A = 1;
    *DC += 1;
}

void parseLabel(char *line, symbol *head, int *IC, int *DC, int *errors, machineCode *mCode) {
    bool isLabel = FALSE;
    bool isDirective = FALSE;
    char *name = (char *) malloc(strlen(line)+1);
    stringCopy(name, line);
    attribute *attribs = (attribute *) malloc(sizeof(attribute));

    symbol *currNode = head;
    symbol *tempNode = (symbol *) malloc(sizeof(symbol));

    isLabel = checkIfLabel(line);
    isDirective = checkIfDirective(line);
    if (isLabel == TRUE && isDirective == TRUE) {
        addSymbol(name, attribs, tempNode, head, currNode, IC);
        codeDataOrString(line, mCode, DC, isLabel);
    } else if (isLabel == FALSE && isDirective == TRUE) {
        codeDataOrString(line, mCode, DC, isLabel);
    }
}


bool firstPass(char *line, FILE *inp, int *errors) {
    int IC = 100;
    int DC = 0;
    machineCode mCode[MAX_COMMANDS];
    fseek(inp, 0, SEEK_SET);
    symbol *head = malloc(sizeof(symbol));

    strcpy(line, iterator(line, inp, errors));
    while (!(strstr(line, "NULL"))) {
        parseLabel(line, head, &IC, &DC, errors, mCode);


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