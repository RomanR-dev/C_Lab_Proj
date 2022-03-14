#ifndef _PARSERS_
#define _PARSERS_

/* parser funcs for asm code */
char **parseWith2Operands(char *input);

char **parseWith1Operand(char *input);

char **parseNoOperands(char *input);

int getOperandsCount(char *cmd, int *errors);

bool assertNumOfOperands(char **parsedLine, int *errors, int numOfOperands);

char **chooseParser(char *input, int *errors);

void parseCmd(char **parsedLine, int *errors, char *cmd, machineCode *mCode, long *IC, char *labelName);

bool isInt(char *num);

bool isValidLabelName(char *label);

bool isLabel(char *operand);

sortType getSortType(char *operand, int *errors);

#endif
