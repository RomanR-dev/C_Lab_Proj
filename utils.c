#include "definitions.h"

/* utils */
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
    while ((IC % 4) != 0) {
        IC--;
    }
    return IC;
}

void addSymbol(char *name, attribute *attribs, symbol *tempNode, symbol *head, symbol *currNode, int *IC) {
    attribs->data = TRUE;
    attribs->code = FALSE;
    attribs->external = FALSE;
    attribs->entry = FALSE;
    name = strtok(name, ":");
    tempNode = setNode(tempNode, name, *IC, findOffset(*IC), *IC - findOffset(*IC), *attribs);
    if (head->isSet == FALSE) {
        copyNode(head, tempNode);
    } else {
        addNodeToList(currNode, tempNode);
    }
}