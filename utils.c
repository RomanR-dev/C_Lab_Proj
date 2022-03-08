#include "definitions.h"
#include "parsers.h"

/* utils */
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

void lstrip(char *l) {
    int i = 0;
    int j = 0;
    while (l[i] == ' ') i++;
    while (l[i] != '\0') {
        l[j] = l[i];
        i++;
        j++;
    }
    l[j] = '\0';
}

void stringCopy(char *dest, char *src) {
    int len = strlen(src);
    int i = 0;
    for (; i < len; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

void delay(int time) {
    int i = 0;
    for (; i < time; i++);
}

FILE *openFile(char *fileName, FILE *inp) {
    char *error = malloc(38 + strlen(fileName));
    char *filePath = malloc(15 + strlen(fileName));
    sprintf(error, "file: %s.as, returned error", fileName);
    sprintf(filePath, "../tester/%s.as", fileName);
    printf("Opening: %s...\n", filePath);
    inp = fopen(filePath, "r");
    if (inp == NULL) {
        perror(error);
        return NULL;
    } else {
        return inp;
    }
}

FILE *inputFileInit(char **argv, FILE *inp, int *inputFileCounter) {
    /* file check and open if exists */
    inp = openFile(argv[*inputFileCounter], inp);
    if (inp == NULL) {
        printf("============================================================\n");
        *inputFileCounter += 1;
        return NULL;
    }
    *inputFileCounter += 1;
    return inp;
}

FILE *outputFileInit(FILE *outP, char *outPutFileName, char *inputName) {
    sprintf(outPutFileName, "../tester/%s.am", inputName);
    printf("Creating output file: %s\n", outPutFileName);
    outP = fopen(outPutFileName, "w+");
    return outP;
}

symbol *setNode(symbol *node, char *name, int IC, int baseAddr, int offset, attribute attribs) {

    node->name = (char *) malloc(strlen(name)); /* TODO: add the name */
    strcpy(node->name, name);

    node->attribs = (attribute *) malloc(sizeof(attribute));
    node->attribs->code = attribs.code;
    node->attribs->data = attribs.data;
    node->attribs->external = attribs.external;
    node->attribs->entry = attribs.entry;

    if (attribs.external == TRUE) {
        node->baseAddress = 0;
        node->offset = 0;
        node->value = 0;
    } else {
        node->baseAddress = baseAddr;
        node->offset = offset;
        node->value = IC;
    }
    node->isSet = TRUE;
    node->hasNext = FALSE;
    return node;
}

void copyNode(symbol *current, symbol *new) {
    current->name = (char *) malloc(strlen(new->name));
    strcpy(current->name, new->name);
    current->attribs = (attribute *) malloc(sizeof(attribute));
    current->attribs->code = new->attribs->code;
    current->attribs->data = new->attribs->data;
    current->attribs->external = new->attribs->external;
    current->attribs->entry = new->attribs->entry;
    current->baseAddress = new->baseAddress;
    current->offset = new->offset;
    current->value = new->value;
    current->isSet = TRUE;
    current->hasNext = new->hasNext;
}

symbol *addNodeToList(symbol *head, symbol *node) {
    while (head->hasNext != FALSE) {
        head = head->next;
    }
    head->next = (symbol *) malloc(sizeof(symbol));
    copyNode(head->next, node);
    head->hasNext = TRUE;
    return head;
}

int findOffset(int IC) {
    IC--;
    while ((IC % 16) != 0) {
        IC--;
    }
    return IC;
}

int checkIfEntryOrExtern(char *line) {
    char *attrib = (char *) malloc(strlen(line) + 1);
    stringCopy(attrib, line);
    attrib = strtok(attrib, " ");
    if (strstr(".entry", attrib)) {
        return 1;
    } else if (strstr(".extern", attrib)) {
        return 2;
    }
    return -1;
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
    char *tempLine = (char *) malloc(strlen(line) + 1);
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

void setAttrib(attribute *attribs, char *line) {
    if (checkIfEntryOrExtern(line) == 1) {
        attribs->data = FALSE;
        attribs->code = FALSE;
        attribs->external = FALSE;
        attribs->entry = TRUE;
    } else if (checkIfEntryOrExtern(line) == 2) {
        attribs->data = FALSE;
        attribs->code = FALSE;
        attribs->external = TRUE;
        attribs->entry = FALSE;
    } else if (checkIfDirective(line) == TRUE){
        attribs->data = TRUE;
        attribs->code = FALSE;
        attribs->external = FALSE;
        attribs->entry = FALSE;
    }
    else {
        attribs->data = FALSE;
        attribs->code = TRUE;
        attribs->external = FALSE;
        attribs->entry = FALSE;
    }
}

void addSymbol(char *name, attribute *attribs, symbol *tempNode, symbol *head, symbol *currNode, int *IC, char *line) {
    int offSet = 0;
    if (*IC != 0) offSet = findOffset(*IC);

    setAttrib(attribs, line);
    name = strtok(name, ":");
    tempNode = setNode(tempNode, name, *IC, offSet, *IC - offSet, *attribs);

    if (head->isSet == FALSE) {
        copyNode(head, tempNode);
    } else {
        addNodeToList(currNode, tempNode);
    }
    *IC += *IC - offSet;
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