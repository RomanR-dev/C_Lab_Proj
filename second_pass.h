#include "misc/definitions.h"
#include "misc/utils.h"
#include "misc/parsers.h"
#include "pre_assembler.h"


void createObFile(char *fileName, machineCode *mCode, int dataCounter, int IC);

void createEntryFile(char *fileName, symbol *head);

void createExternFile(char *fileName, symbol *head, machineCode *mCode);

void createOutPutFileNames(char *orig, char *ext, char *ent, char *ob);

bool createOutPutFiles(machineCode *mCode, symbol *head, char *outPutFileName, int dataCounter, int IC);

void entryStep(char *line, symbol *head);

void fillBlanks(symbol *tempNode, machineCode *mCode, int i, char *labelUsage);

bool secondPass(char *line, FILE *inp, int *errors, symbol *head, machineCode *mCode, long *IC, long *DC,
                char *outPutFileName, int *dataCounter);