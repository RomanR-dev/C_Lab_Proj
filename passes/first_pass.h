#include "../misc/definitions.h"
#include "../misc/utils.h"
#include "../misc/parsers.h"
#include "pre_assembler.h"


int codeDataOrString(char *line, machineCode *mCode, long *DC, bool withLabel, const long *IC, int *errors);

bool labelAndDirectiveStep(char *line, symbol *head, long *IC, long *DC,
                           int *errors, machineCode *mCode, int *DCF, int *dataCounter);

void externStep(char *line, symbol *head, int *errors, long IC);

void alignTables(int ICF, symbol *head, machineCode *mCode);

bool firstPass(char *line, FILE *inp, int *errors, char *outPutFileName);
