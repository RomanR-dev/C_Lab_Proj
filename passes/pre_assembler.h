#include "../misc/definitions.h"
#include "../misc/utils.h"
#include "../misc/parsers.h"




macroTable *addMacro(macroTable *table, char *name, char **cmd, int numOfCmds);

bool macroWriter(char *line, macroTable *table, FILE *outP);

void preAssembler(char *line, int *errors, FILE *inp, FILE *outP, macroTable *table);