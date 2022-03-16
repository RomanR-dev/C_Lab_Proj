#include "definitions.h"
#include "parsers.h"

/* utils */
/**
 * swap last char /n for /0
 * @param string
 */
void swapLastCharIfNewLine(char *string) {
    int len;
    len = strlen(string);
    if (string[len] == '\n') {
        string[len] = '\0';
        return;
    }
    if (string[len - 1] == '\n') {
        string[len - 1] = '\0';
        return;
    }
}

/**
 * strip leading whitespaces
 * @param l
 */
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

/**
 * copy string from src to dest
 * @param dest
 * @param src
 */
void stringCopy(char *dest, char *src) {
    int len = strlen(src);
    int i = 0;
    for (; i < len; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

/**
 * open input file
 * @param fileName
 * @param inp
 * @return
 */
FILE *openFile(char *fileName, FILE *inp) {
    char *error = malloc(40 + strlen(fileName));
    char *filePath = malloc(17 + strlen(fileName));
    sprintf(error, "file: %s.as, returned error", fileName);
    sprintf(filePath, "%s.as", fileName);
    printf("Processing: %s\n", filePath);

    inp = fopen(filePath, "r");
    if (inp == NULL) {
        perror(error);
        return NULL;
    } else {
        return inp;
    }
    return inp;
}

/**
 * proces input file and inc file counter
 * @param argv
 * @param inp
 * @param inputFileCounter
 * @return
 */
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

/**
 * create output file .am
 * @param outP
 * @param outPutFileName
 * @param inputName
 * @return
 */
FILE *outputFileInit(FILE *outP, char *outPutFileName, char *inputName) {
    sprintf(outPutFileName, "tester/%s.am", inputName);
    outP = fopen(outPutFileName, "w+");
    return outP;
}

/**
 * set node in the linked list (symbols table)
 * @param node
 * @param name
 * @param IC
 * @param baseAddr
 * @param offset
 * @param attribs
 * @return
 */
symbol *setNode(symbol *node, char *name, int IC, int baseAddr, int offset, attribute attribs) {

    node->name = (char *) malloc(strlen(name)); /* TODO: add the name */
    stringCopy(node->name, name);

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

/**
 * copy node from 1 to another 2 (deep copy)
 * @param current
 * @param new
 */
void copyNode(symbol *current, symbol *new) {
    current->name = (char *) malloc(strlen(new->name) + 1);
    stringCopy(current->name, new->name);
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

/**
 * add node to linked list
 * @param head
 * @param node
 * @return
 */
symbol *addNodeToList(symbol *head, symbol *node) {
    while (head->hasNext != FALSE) {
        head = head->next;
    }
    head->next = (symbol *) malloc(sizeof(symbol));
    copyNode(head->next, node);
    head->hasNext = TRUE;
    return head;
}

/**
 * check if malloc was successful
 * @param ptr
 * @return
 */
bool checkMalloc(void *ptr) {
    if (ptr == NULL) {
        printf("--->malloc failed\n");
        return FALSE;
    }
    return TRUE;
}

/**
 * find offset for symbols table
 * @param IC
 * @return
 */
int findOffset(int IC) {
    IC--;
    while ((IC % 16) != 0) {
        IC--;
    }
    return IC;
}

/**
 * check if derective is entry or extern
 * @param line
 * @return
 */
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

/**
 * check if attribute
 * @param attrib
 * @return
 */
int checkIfAttrib(char *attrib) {
    if (strstr(".entry", attrib) || strstr(".extern", attrib)) {
        return 1;
    } else if (strstr(".data", attrib) || strstr(".string", attrib)) {
        return 2;
    }
    return 0;
}

/**
 * check if received a directive command
 * @param line
 * @return
 */
bool checkIfDirective(char *line) {
    int res;
    char *tempLine = (char *) malloc(strlen(line) + 1);
    stringCopy(tempLine, line);
    swapLastCharIfNewLine(tempLine);
    tempLine = strtok(tempLine, " ");
    if (tempLine != NULL && tempLine[0] == '.') {
        res = checkIfAttrib(tempLine); /* check if attrib*/
        if (res == 2) {
            return TRUE;
        }
    }
    tempLine = strtok(NULL, " ");
    if (tempLine != NULL && tempLine[0] == '.') {
        res = checkIfAttrib(tempLine); /* check if attrib*/
        if (res == 2) {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * set attributes in the node
 * @param attribs
 * @param line
 */
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
    } else if (checkIfDirective(line) == TRUE) {
        attribs->data = TRUE;
        attribs->code = FALSE;
        attribs->external = FALSE;
        attribs->entry = FALSE;
    } else {
        attribs->data = FALSE;
        attribs->code = TRUE;
        attribs->external = FALSE;
        attribs->entry = FALSE;
    }
}

/**
 * add new symbol to the symbols table
 * @param name
 * @param attribs
 * @param tempNode
 * @param head
 * @param currNode
 * @param IC
 * @param line
 */
void addSymbol(char *name, attribute *attribs, symbol *tempNode, symbol *head, symbol *currNode, long IC, char *line) {
    int offSet = 0;
    if (IC != 0) offSet = findOffset(IC);

    setAttrib(attribs, line);
    name = strtok(name, ":");
    tempNode = setNode(tempNode, name, IC, offSet, IC - offSet, *attribs);

    if (head->isSet != TRUE) {
        copyNode(head, tempNode);
    } else {
        addNodeToList(currNode, tempNode);
    }
    /* *IC += *IC - offSet;*/
}

/**
 * get next line
 * @param line
 * @param inp
 * @param errors
 * @return next line char array
 */
char *iterator(char *line, FILE *inp, const int *errors) {
    while ((fgets(line, MAX_LENGTH, inp)) != NULL) { /* read from .as file and save macro data to .am file */
        if (strlen(line) > MAX_LENGTH) {
            errors += 1;
            printf("--->line to long\n");
            continue;
        }
        if (line[0] == '\n') continue;
        if (line[0] == ';') continue;
        lstrip(line);
        return line;
    }
    return "NULL";
}

/**
 * print error wrapper
 * @param error
 */
void printError(char *error) {
    printf("\n--->%s\n", error);
}

/**
 * set A, R, E bits
 * @param IC
 * @param mCode
 * @param A
 * @param R
 * @param E
 */
void setARE(int IC, machineCode *mCode, unsigned char A, unsigned char R, unsigned char E) {
    mCode[IC].word.data->A = A;
    mCode[IC].word.data->R = R;
    mCode[IC].word.data->E = E;
}

/**
 * add additional lines based on source and dest sort
 * @param mCode
 * @param IC
 * @param sort
 * @param L
 * @param operand
 */
void setAdditionalLines(machineCode *mCode, long *IC, sortType sort, int *L, char *operand) {
    long num;
    if (sort == sort0) {
        mCode[*IC].word.data = (word2 *) malloc(sizeof(word2));
        checkMalloc(mCode[*IC].word.data);
        mCode[*IC].set = 'd';
        setARE(*IC, mCode, 1, 0, 0);
        num = strtol(++operand, NULL, 10);
        mCode[*IC].word.data->opcode = num;
        *L += 1;
        *IC += 1;
    } else if (sort == sort1 || sort == sort2) {
        mCode[*IC].word.data = (word2 *) malloc(sizeof(word1));
        checkMalloc(mCode[*IC].word.data);
        mCode[*IC].set = 'd';
        setARE(*IC, mCode, 0, 0, 0);
        mCode[*IC].word.data->opcode = '?';/*dealt at 2nd pass*/
        mCode[*IC].additionalLine = TRUE;
        *L += 1;
        *IC += 1;
        mCode[*IC].word.data = (word2 *) malloc(sizeof(word2));
        checkMalloc(mCode[*IC].word.data);
        setARE(*IC, mCode, 0, 0, 0);
        mCode[*IC].set = 'd';
        mCode[*IC].word.data->opcode = '?';
        mCode[*IC].additionalLine = TRUE;
        *L += 1;
        *IC += 1;
    }
}

/**
 * extract register number
 * @param reg
 * @param sort
 * @return
 */
int regNumber(char *reg, sortType sort) {
    if (sort == sort2) {
        strtok(reg, "[");
        reg = strtok(NULL, ""); /* for regs in []*/
        reg[strlen(reg) - 1] = '\0';
    }
    ++reg;
    return atoi(reg);
}

/**
 * set operand label if one of the operands is a label
 * @param destSort
 * @param sourceSort
 * @param labelName
 * @param mCode
 * @param parsedLine
 * @param IC
 * @param operands
 */
void setOperandLabel(sortType destSort, sortType sourceSort, const char *labelName,
                     machineCode *mCode, char **parsedLine, const long *IC, int operands) {
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

/**
 * set code line in machine code mem snapshot
 * @param mCode
 * @param IC
 * @param f
 * @param parsedLine
 * @param labelName
 * @param errors
 */
void setCode(machineCode *mCode, long *IC, func *f, char **parsedLine, char *labelName, int *errors) {
    int L = 0;
    sortType sourceSort = unsorted;
    sortType destSort = unsorted;
    unsigned int destReg;
    unsigned int sourceReg;
    mCode[*IC].word.data = (word2 *) malloc(sizeof(word2));
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
    mCode[*IC].word.code = (word1 *) malloc(sizeof(word1));
    checkMalloc(mCode[*IC].word.code);
    mCode[*IC].set = 'c';
    mCode[*IC].word.code->funct = f->funct;
    if (f->operands == 1) {
        setARE(*IC, mCode, 1, 0, 0);
        mCode[*IC].word.code->sourceSort = 0;
        mCode[*IC].word.code->destReg = 0;
        mCode[*IC].word.code->sourceReg = 0;

        destSort = getSortType(parsedLine[1], errors);
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

        sourceSort = getSortType(parsedLine[1], errors);
        destSort = getSortType(parsedLine[2], errors);
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

/**
 * receive a line a check for errors
 * @param errors
 * @param currLine
 */
void errorHandler(int *errors, char *currLine) {
    char *lineForErrorHandling;
    lineForErrorHandling = (char *) malloc(strlen(currLine) + 1);
    checkMalloc(lineForErrorHandling);
    stringCopy(lineForErrorHandling, currLine);
}

/**
 * power math func
 * @param num
 * @param times
 * @return
 */
long power(int num, int times) {
    if (times > 1) {
        return num * power(num, times - 1);
    }
    return num;
}

/**
 * convert binary 4 bit num to hex
 * @param binNumber
 * @param bit
 * @return
 */
long convertBinToHex4Bit(const char *binNumber, int bit) {
    long dec = 0;
    int i;
    for (i = bit - 1; i >= 0; i--) {
        if (binNumber[i] == '1') {
            if (i == bit - 1) {
                dec += 1;
            } else {
                dec += power(2, bit - i - 1);
            }
        }
    }
    return dec;
}

/**
 * convert binary 16 bit to hex
 * @param binNumber
 * @param bit
 * @return
 */
long convertBinToHex16Bit(const char *binNumber, int bit) {
    long dec = 0;
    int i;
    for (i = bit; i > -1; i--) {
        if (binNumber[i] == '1') {
            if (i == 0) {
                dec += 1;
            } else {
                dec += power(2, i);
            }
        }
    }
    return dec;
}

/**
 * convert integer type to char
 * @param num
 * @return
 */
char intToChar(unsigned int num) {
    return (char) (num + '0');
}

/**
 * convert bin to hex
 * @param binNumber
 * @param bit
 * @return
 */
long convertBinToHex(char *binNumber, int bit) {
    if (bit == 4) {
        return convertBinToHex4Bit(binNumber, 4);
    }
    if (bit == 16) {
        return convertBinToHex16Bit(binNumber, 16);
    }
    return -1;
}

/**
 * get how many times to multiply the number by itself for conversion
 * @param number
 * @return
 */
int getTimesToPower(int number) {
    if (number == 0 || number == 4 || number == 8 || number == 12) {
        return 0;
    }
    if (number == 1 || number == 5 || number == 9 || number == 13) {
        return 1;
    }
    if (number == 2 || number == 6 || number == 10 || number == 14) {
        return 2;
    }
    if (number == 3 || number == 7 || number == 11 || number == 15) {
        return 2;
    }
    return 0;
}

/**
 * get which number corresponds in 4 bit binary
 * @param number
 * @return
 */
int getTheNumberIn4BitSection(unsigned int number) {
    if (number == 0 || number == 4 || number == 8 || number == 12) {
        return 0;
    }
    if (number == 1 || number == 5 || number == 9 || number == 13) {
        return 1;
    }
    if (number == 2 || number == 6 || number == 10 || number == 14) {
        return 2;
    }
    if (number == 3 || number == 7 || number == 11 || number == 15) {
        return 3;
    }
    return -1;
}

/**
 * get which number corresponds in 16 bit binary
 * @param number
 * @return
 */
int get16BitWordSection(unsigned int number) {
    if (number == 0 || number == 1 || number == 2 || number == 3) {
        return 16;
    }
    if (number == 4 || number == 5 || number == 6 || number == 7) {
        return 12;
    }
    if (number == 8 || number == 9 || number == 10 || number == 11) {
        return 8;
    }
    if (number == 12 || number == 13 || number == 14 || number == 15) {
        return 4;
    }
    return 16;
}

/**
 * convert decimal to 16 or 4 bit binary char array
 * @param binNumber
 * @param number
 * @param isAdditionalLine
 * @param type
 * @return
 */
char *decToBin(char *binNumber, unsigned int number, bool isAdditionalLine, char type) {
    int num1, num2, len, i;
    if (number == 0 && type == 'c') {
        for (i = 0; i < strlen(binNumber); i++) {
            binNumber[i] = '0';
        }
        binNumber[i + 1] = '\0';
        return binNumber;
    }
    len = get16BitWordSection(number);
    if (isAdditionalLine != TRUE && type == 'd') {
        num1 = getTheNumberIn4BitSection(number);
        if (num1 != -1) {
            num2 = getTimesToPower(number);
            number = power(num2, num1);
            if (num1 == 0 && num2 == 0) number = 1;
            if (num1 == 1 && num2 == 1) number = 2;
        }

    } else if (isAdditionalLine == TRUE || type == 'D') {
        len = 16;
    }
    if (strlen(binNumber) == 4) len = 4;
    for (i = len - 1; number > 0; i--) {
        binNumber[i] = intToChar(number % 2);
        number = number / 2;
    }
    return binNumber;
}

/**
 * assign the bin number to corresponding location in 16 bit divided to 4 4's
 * @param binNumber
 * @param binNumber16
 * @param start
 * @param letter
 * @return
 */
long assign4BitBinNumber(char *binNumber, const char *binNumber16, int start, long letter) {
    int i = 0;
    for (; i < 4; i++, start++) {
        binNumber[i] = binNumber16[start];
    }
    letter = convertBinToHex(binNumber, 4);
    return letter;
}

/**
 * reset received array
 * @param array
 * @param size
 */
void resetArray(char *array, int size) {
    memset(array, '0', size);
    array[size] = '\0';
}

/**
 * reset array used for 4 and 16 bit repr
 * @param binNumber
 * @param binNumber16
 */
void resetArrays(char *binNumber, char *binNumber16) {
    resetArray(binNumber, 4);
    resetArray(binNumber16, 16);
}

/**
 * free mallocs in machine codes memory snapshot array
 * @param mCode
 * @param IC
 */
void freeMachineCodes(machineCode *mCode, int IC) {
    int counter = 0;
    for (; counter < IC; counter++) {
        mCode[counter].set = '0';
        mCode[counter].L = 0;
        mCode[counter].additionalLine = 0;
        mCode[counter].labelUsageDest = NULL;
        mCode[counter].labelUsageSource = NULL;
        mCode[counter].declaredLabel = NULL;
        mCode[counter].isDataOrString = 0;
        if (mCode[counter].word.data != NULL) {
            mCode[counter].word.data->opcode = 0;
            mCode[counter].word.data->A = 0;
            mCode[counter].word.data->R = 0;
            mCode[counter].word.data->E = 0;
        }
        if (mCode[counter].word.code != NULL) {
            mCode[counter].word.code->funct = 0;
            mCode[counter].word.code->A = 0;
            mCode[counter].word.code->R = 0;
            mCode[counter].word.code->E = 0;
            mCode[counter].word.code->destReg = 0;
            mCode[counter].word.code->sourceReg = 0;
            mCode[counter].word.code->destSort = 0;
            mCode[counter].word.code->sourceSort = 0;
        }
    }
}

/**
 * free mallocs from 2 passes
 * @param mCode
 * @param head
 * @param IC
 */
void freeMallocsFromPasses(machineCode *mCode, symbol *head, int IC) {
    freeMachineCodes(mCode, IC);
    free(head);
}
