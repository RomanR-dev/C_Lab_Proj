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
    char *currLine = malloc(strlen(line));
    int macroCounter = 0;
    int i = 0;
    stringCopy(currLine, line);
    while (table[macroCounter].set == 1) {
        if (strstr(currLine, table[macroCounter].name)) {
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

void setAttribType(char *attrib, attribute *att) {
    if (strstr(attrib, ".entry")) {
        att->entry = TRUE;
    } else if (strstr(attrib, ".extern")) {
        att->external = TRUE;
    } else if (strstr(attrib, ".data")) {
        att->data = TRUE;
    } else {
        att->code = TRUE;
    }
}

bool checkIfAttrib(char *attrib) {
    if (strstr(".entry", attrib) || strstr(".extern", attrib) || strstr(".data", attrib)) {
        return TRUE;
    }
    return FALSE;
}

symbol *setNode(symbol *node, char *name, int value, int baseAddr, int offset, attribute attribs) {

    node->name = (char *) malloc(strlen(name)); /* TODO: add the name */
    strcpy(node->name, name);

    node->attribs = (attribute *) malloc(sizeof(attribute));
    node->attribs->code = attribs.code;
    node->attribs->data = attribs.data;
    node->attribs->external = attribs.external;
    node->attribs->entry = attribs.entry;

    node->baseAddress = baseAddr ? attribs.external == 0 : 0;
    node->offset = offset ? attribs.external == 0 : 0;
    node->value = value ? attribs.external == 0 : 0;
    node->isSet = TRUE;
    return node;

}

int checkIfLabel(char *line, attribute *attrib) {
    char *ptr = line;
    int isAttrib;
    ptr = strtok(ptr, " ");
    if (ptr[strlen(ptr) - 1] == ':') {
        /* check whether there is code after */
        return 1;
    } else if (ptr[0] == '.') {
        isAttrib = checkIfAttrib(ptr); /* check if attrib*/
        if (isAttrib == TRUE) {
            setAttribType(ptr, attrib);
            return 2;
        }
    }
    return 0;
}

int findOffset(int IC) {
    int res;
    while ((res = IC % 4) != 0) {
        IC--;
    }
    return res;
}

void parseLabel(char *line, symbol *head, int IC) {
    int res;
    char *name = line;
    attribute *attribs = (attribute *) malloc(sizeof(attribute));
    symbol *currNode = head;
    symbol *tempNode = (symbol *) malloc(sizeof(symbol));
    res = checkIfLabel(line, attribs);
    /*TODO run to the latest unused node in linked list*/
    if (res == 1) {
        name = strtok(name, ":");
        tempNode = setNode(tempNode, name, IC, findOffset(IC), IC - findOffset(IC), *attribs);
    } else if (res == 2) {
        name = strtok(name, " ");
        name = strtok(NULL, " ");
        tempNode = setNode(tempNode, name, IC, findOffset(IC), IC - findOffset(IC), *attribs);
    };
}


void preAssembler(char *line, int *errors, FILE *inp, FILE *outP, macroTable *table) {
    int foundMacro = 0;
    bool endm = false;
    bool writeResult;
    /* macro finder  */
    if (strstr(line, "macro")) {
        foundMacro = 1;
        char **tempLine = malloc(1);
        char *macroName = malloc(74);
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
                endm = true;
                table = addMacro(table, macroName, tempLine, counter);
                foundMacro = 0;
                break;
            }
            lstrip(line);
            tempLine[counter] = malloc(sizeof(char) * strlen(line));
            stringCopy(tempLine[counter], line);
            counter++;
        }
        if (foundMacro == 1) {
            errors += 1;
        }
    }
    /* if written macro continue to next line, else write the line as is to new file */
    writeResult = macroWriter(line, table, outP);
    if (!writeResult && endm == false) fputs(line, outP);
}

char *iterator(char *line, FILE *inp, int *errors) {
    while ((fgets(line, 80, inp)) != NULL) { /* read from .as file and save macro data to .am file */
        if (strlen(line) > 80) {
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
    char line[80];
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
            /*firstPass(line, outP, &errors);*/
        } else {
            fclose(outP);
            fclose(inp);
            perror("due to errors not continuing with flow on current file, continue with next file...");
        }
        printf("============================================================\n");
    }
}

bool firstPass(char *line, FILE *inp, int *errors) {
    symbol *head = malloc(sizeof(symbol));

    strcpy(line, iterator(line, inp, errors));
    while (!(strstr(line, "NULL"))) {
        strcpy(line, iterator(line, inp, errors));

    }

    /* check if label in said line */
    printf("first pass finisedh with %d errors", *errors);
    if (errors == 0) {
        fclose(inp);
        /*secondPass(line, outP, &errors);*/
    } else {
        fclose(inp);
        perror("due to errors not continuing with flow on current file, continue with next file...");
    }
    printf("============================================================\n");
    return true;
}


char **secondPass() {
    char **parsedLine;


    return parsedLine;
}


int main(int argc, char **argv) {
    int IC = 100;
    int DC = 0;

    mainRunner(argc, argv); /* first pass returns list of .am file names, updates amount in newArgc*/




//    free(newArgv);
    return 0;
}