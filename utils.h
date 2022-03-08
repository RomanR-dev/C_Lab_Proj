#ifndef _UTILS_
#define _UTILS_

/* utils */
void lstrip(char *l);

void stringCopy(char *dest, char *src);

FILE *openFile(char *fileName, FILE *inp);

FILE *inputFileInit(char **argv, FILE *inp, int *inputFileCounter);

FILE *outputFileInit(FILE *outP, char *outPutFileName, char *inputName);

void delay(int time); /* not necessary */

void addSymbol(char *name, attribute *attribs, symbol *tempNode,
               symbol *head, symbol *currNode, int *IC, char *line);

char *iterator(char *line, FILE *inp, int *errors);

bool lineSplitterFuncAndConcatenate(char **parsedLine, int opCount, char *line, int errors);

void printWrittenLine(int opCount, char **parsedLine);

char *concatenate(char **toConCat, int max);

bool checkIfDirective(char *line);

int checkIfAttrib(char *attrib);

int checkIfEntryOrExtern(char *line);

#endif

