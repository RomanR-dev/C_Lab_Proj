#ifndef _UTILS_
#define _UTILS_

int lineNum;
/* utils */
void swapLastCharIfNewLine(char *string);

void lstrip(char *l);

void stringCopy(char *dest, char *src);

FILE *openFile(char *fileName, FILE *inp);

FILE *inputFileInit(char **argv, FILE *inp, int *inputFileCounter);

FILE *outputFileInit(FILE *outP, char *outPutFileName, char *inputName);

symbol *setNode(symbol *node, char *name, int IC, int baseAddr, int offset, attribute attribs);

void copyNode(symbol *current, symbol *new);

symbol *addNodeToList(symbol *head, symbol *node);

bool checkMalloc(void *ptr);

int findOffset(int IC);

int checkIfEntryOrExtern(char *line);

int checkIfAttrib(char *attrib);

bool checkIfDirective(char *line);

void setAttrib(attribute *attribs, char *line);

void addSymbol(char *name, attribute *attribs, symbol *tempNode, symbol *head, symbol *currNode, long IC, char *line);

char *iterator(char *line, FILE *inp, const int *errors);

void printError(char *error);

void setARE(int IC, machineCode *mCode, unsigned char A, unsigned char R, unsigned char E);

void setAdditionalLines(machineCode *mCode, long *IC, sortType sort, int *L, char *operand);

int regNumber(char *reg, sortType sort, int *errors);

void setOperandLabel(sortType destSort, sortType sourceSort, const char *labelName,
                     machineCode *mCode, char **parsedLine, const long *IC, int operands);

void setCode(machineCode *mCode, long *IC, func *f, char **parsedLine, char *labelName, int *errors);

bool checkIfLabel(char *line);

void errorHandler(int *errors, char *currLine);

long power(int num, int times);

long convertBinToHex4Bit(const char *binNumber, int bit);

long convertBinToHex16Bit(const char *binNumber, int bit);

char intToChar(unsigned int num);

long convertBinToHex(char *binNumber, int bit);

char *decToBin(char *binNumber, unsigned int number, bool isAdditionalLine, char type);

long assign4BitBinNumber(char *binNumber, const char *binNumber16, int start, long letter);

void resetArray(char *array, int size);

void resetArrays(char *binNumber, char *binNumber16);

void freeMachineCodes(machineCode *mCode, int IC);

void freeMallocsFromPasses(machineCode *mCode, symbol *head, int IC);

#endif

