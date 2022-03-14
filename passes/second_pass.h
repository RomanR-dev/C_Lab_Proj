#include "../misc/definitions.h"
#include "../misc/utils.h"
#include "../misc/parsers.h"
#include "pre_assembler.h"

void set19_16_bit_data(char *binNumber, machineCode *mCode, int i);

void set19_16_bit_code(char *binNumber, machineCode *mCode, int i);

void set15_0_bit_data(char *binNumber, char *binNumber16, machineCode *mCode, int i,
                      long *A, long *B, long *C, long *D, long *E);

void set15_0_bit_code(char *binNumber16, char *binNumber, long *B, long *C, long *D, long *E);

void setDataTo16BitWord(char *binNumber16, const char *binNumber, int start16, int start4, int end);

void prepare15_to_0_bits(char *binNumber16, char *binNumber, machineCode *mCode, int i);

void createObFile(char *fileName, machineCode *mCode, int dataCounter, int IC);

void createEntryFile(char *fileName, symbol *head);

void createExternFile(char *fileName, symbol *head, machineCode *mCode);

void createOutPutFileNames(char *orig, char *ext, char *ent, char *ob);

bool createOutPutFiles(machineCode *mCode, symbol *head, char *outPutFileName, int dataCounter, int IC);

void entryStep(char *line, symbol *head);

void fillBlanks(symbol *tempNode, machineCode *mCode, int i, char *labelUsage);

bool checkLabel(const char *label, symbol *tempNode, bool found, int *errors);

void assertLabelsDeclaration(int *errors, machineCode *mCode, const long *IC, symbol *head);

bool secondPass(char *line, FILE *inp, int *errors, symbol *head, machineCode *mCode, const long *IC,
                char *outPutFileName, int *dataCounter);
