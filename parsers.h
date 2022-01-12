

int getOperandsCount(char * cmd, int * errors);

char ** parseWith2Operands(char * input);

char ** parseWith1Operand(char * input);

char ** parseNoOperands(char * input);

char ** chooseParser(char * input, int * errors);