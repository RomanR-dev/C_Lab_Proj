#include "../misc/definitions.h"
#include "../misc/utils.h"
#include "pre_assembler.h"

void set19_16_bit_data(char *binNumber, machineCode *mCode, int i) {
    binNumber[0] = '0';
    binNumber[1] = intToChar(mCode[i].word.data->A);
    binNumber[2] = intToChar(mCode[i].word.data->R);
    binNumber[3] = intToChar(mCode[i].word.data->E);
}

void set19_16_bit_code(char *binNumber, machineCode *mCode, int i) {
    binNumber[0] = '0';
    binNumber[1] = intToChar(mCode[i].word.code->A);
    binNumber[2] = intToChar(mCode[i].word.code->R);
    binNumber[3] = intToChar(mCode[i].word.code->E);
}

void set15_0_bit_data(char *binNumber, char *binNumber16, machineCode *mCode, int i,
                      long *A, long *B, long *C, long *D, long *E) {
    binNumber16 = decToBin(binNumber16, mCode[i].word.data->opcode, mCode[i].additionalLine);
    *A = convertBinToHex(binNumber, 4);
    resetArray(binNumber, 4);
    *B = assign4BitBinNumber(binNumber, binNumber16, 0, *B);
    resetArray(binNumber, 4);
    *C = assign4BitBinNumber(binNumber, binNumber16, 4, *C);
    resetArray(binNumber, 4);
    *D = assign4BitBinNumber(binNumber, binNumber16, 8, *D);
    resetArray(binNumber, 4);
    *E = assign4BitBinNumber(binNumber, binNumber16, 12, *E);
    resetArray(binNumber, 4);
}

void set15_0_bit_code(char *binNumber16, char *binNumber, long *B, long *C, long *D, long *E) {
    *B = assign4BitBinNumber(binNumber, binNumber16, 0, *B);
    resetArray(binNumber, 4);
    *C = assign4BitBinNumber(binNumber, binNumber16, 4, *C);
    resetArray(binNumber, 4);
    *D = assign4BitBinNumber(binNumber, binNumber16, 8, *D);
    resetArray(binNumber, 4);
    *E = assign4BitBinNumber(binNumber, binNumber16, 12, *E);
    resetArray(binNumber, 4);
}

void setDataTo16BitWord(char *binNumber16, const char *binNumber, int start16, int start4, int end) {
    int i = 0;
    for (; end > 0; end--, i++) {
        binNumber16[start16 + i] = binNumber[start4 + i];
    }
}

void prepare15_to_0_bits(char *binNumber16, char *binNumber, machineCode *mCode, int i) {
    resetArray(binNumber, 4);
    binNumber = decToBin(binNumber, mCode[i].word.code->funct, TRUE);
    setDataTo16BitWord(binNumber16, binNumber, 0, 0, 4);

    resetArray(binNumber, 4);
    binNumber = decToBin(binNumber, mCode[i].word.code->sourceReg, TRUE);
    setDataTo16BitWord(binNumber16, binNumber, 4, 0, 4);

    resetArray(binNumber, 4);
    binNumber = decToBin(binNumber, mCode[i].word.code->sourceSort, TRUE);
    setDataTo16BitWord(binNumber16, binNumber, 8, 2, 2);

    resetArray(binNumber, 4);
    binNumber = decToBin(binNumber, mCode[i].word.code->destReg, TRUE);
    setDataTo16BitWord(binNumber16, binNumber, 10, 0, 4);

    resetArray(binNumber, 4);
    binNumber = decToBin(binNumber, mCode[i].word.code->destSort, TRUE);
    setDataTo16BitWord(binNumber16, binNumber, 14, 2, 2);

    resetArray(binNumber, 4);
}

void createObFile(char *fileName, machineCode *mCode, int dataCounter, int IC) {
    int i = 100;
    long A, B, C, D, E;
    char *binNumber = (char *) malloc(sizeof(char) * 4);
    char *binNumber16 = (char *) malloc(sizeof(char) * 16);
    FILE *file = fopen(fileName, "w+");
    fprintf(file, "%d %d\n", IC - dataCounter - 99, dataCounter);
    while (i <= IC && mCode[i].set != '0') {
        resetArrays(binNumber, binNumber16);
        if (mCode[i].set == 'd' || mCode[i].set == 'D') {
            set19_16_bit_data(binNumber, mCode, i);
            set15_0_bit_data(binNumber, binNumber16, mCode, i, &A, &B, &C, &D, &E);
        } else if (mCode[i].set == 'c') {
            set19_16_bit_code(binNumber, mCode, i);
            A = convertBinToHex(binNumber, 4);
            prepare15_to_0_bits(binNumber16, binNumber, mCode, i);
            set15_0_bit_code(binNumber16, binNumber, &B, &C, &D, &E);
        }
        fprintf(file, "%04d    A%lx-B%lx-C%lx-D%lx-E%lx\n", i, A, B, C, D, E);
        i++;
    }
    fclose(file);
}

void createEntryFile(char *fileName, symbol *head) {
    bool createdFile = FALSE;
    FILE *file;
    symbol *tempNode = head;
    while (tempNode->isSet == TRUE) {
        if (tempNode->attribs->entry == TRUE) {
            if (createdFile == FALSE) {
                file = fopen(fileName, "w+");
                createdFile = TRUE;
            }
            fprintf(file, "%s,%d,%d\n", tempNode->name, tempNode->baseAddress, tempNode->offset);
        }
        if (tempNode->hasNext == TRUE) {
            tempNode = tempNode->next;
        } else {
            break;
        }
    }
    if (createdFile == TRUE) {
        fclose(file);
    }
}

void createExternFile(char *fileName, symbol *head, machineCode *mCode) {
    FILE *file;
    bool createdFile = FALSE;
    symbol *tempNode = head;
    int i;
    int base;
    int offset;
    while (tempNode->isSet == TRUE) {
        if (tempNode->attribs->external == TRUE) {
            if (createdFile == FALSE) {
                file = fopen(fileName, "w+");
                createdFile = TRUE;
            }
            i = 100;
            while (mCode[i].set != '\0') {
                if (mCode[i].labelUsageDest != NULL && strstr(mCode[i].labelUsageDest, tempNode->name)) {
                    while (mCode[i].L == 0) {
                        i++;
                    }
                    base = i - 1;
                    offset = base + 1;
                }
                if (mCode[i].labelUsageSource != NULL && strstr(mCode[i].labelUsageSource, tempNode->name)) {
                    base = i + 2;
                    offset = base + 1;
                }
                i++;
            }
            fprintf(file, "%s BASE %d\n%s OFFSET %d\n\n", tempNode->name, base, tempNode->name, offset);
        }
        if (tempNode->hasNext == TRUE) {
            tempNode = tempNode->next;
        } else {
            break;
        }
    }
    if (createdFile == TRUE) {
        fclose(file);
    }
}

void createOutPutFileNames(char *orig, char *ext, char *ent, char *ob) {
    int len;
    stringCopy(ext, orig);
    stringCopy(ent, orig);
    stringCopy(ob, orig);
    len = strlen(ext);
    ext[len - 2] = 'e';
    ext[len - 1] = 'x';
    ext[len] = 't';
    ext[len + 1] = '\0';
    ent[len - 2] = 'e';
    ent[len - 1] = 'n';
    ent[len] = 't';
    ent[len + 1] = '\0';
    ob[len - 2] = 'o';
    ob[len - 1] = 'b';

}

bool createOutPutFiles(machineCode *mCode, symbol *head, char *outPutFileName, int dataCounter, int IC) {
    char *extFileName = (char *) malloc(strlen(outPutFileName) + 1);
    char *entFileName = (char *) malloc(strlen(outPutFileName) + 1);
    char *obFileName = (char *) malloc(strlen(outPutFileName));
    createOutPutFileNames(outPutFileName, extFileName, entFileName, obFileName);
    createEntryFile(entFileName, head);
    createExternFile(extFileName, head, mCode);
    createObFile(obFileName, mCode, dataCounter, IC);
    return TRUE;
}

void entryStep(char *line, symbol *head) {
    char *tempLine = (char *) malloc(strlen(line) + 1);
    symbol *temp;
    temp = head;
    checkMalloc(tempLine);
    stringCopy(tempLine, line);
    strtok(tempLine, " ");
    tempLine = strtok(NULL, "");
    if (tempLine[strlen(tempLine) - 1] == ' ' || tempLine[strlen(tempLine) - 1] == '\n') {
        tempLine[strlen(tempLine) - 1] = '\0';
    }
    while (temp->isSet == TRUE) {
        if (strstr(temp->name, tempLine)) {
            temp->attribs->entry = TRUE;
            break;
        }
        if (temp->hasNext == TRUE) {
            temp = temp->next;
        } else {
            break;
        }
    }
}

void fillBlanks(symbol *tempNode, machineCode *mCode, int i, char *labelUsage) {
    while (tempNode->isSet == TRUE) {
        if (labelUsage != NULL && strstr(labelUsage, tempNode->name)) {
            while (mCode[i].L == 0) {
                i++;
                if (mCode[i].word.data->opcode == 63) { /*check if ?????? in line*/
                    mCode[i].word.data->opcode = tempNode->baseAddress;
                    mCode[i + 1].word.data->opcode = tempNode->offset;
                    if (tempNode->attribs->external == TRUE) {
                        setARE(i, mCode, 0, 0, 1);
                        setARE(i + 1, mCode, 0, 0, 1);
                    } else {
                        setARE(i, mCode, 0, 1, 0);
                        setARE(i + 1, mCode, 0, 1, 0);
                    }
                }
            }
        }
        if (tempNode->hasNext == TRUE) {
            tempNode = tempNode->next;
        } else {
            break;
        }
    }
}

bool secondPass(char *line, FILE *inp, int *errors, symbol *head, machineCode *mCode, const long *IC,
                char *outPutFileName, const int *dataCounter) {
    int i = 0;
    symbol *tempNode = head;
    fseek(inp, 0, SEEK_SET);
    stringCopy(line, iterator(line, inp, errors));
    while (!(strstr(line, "NULL"))) {
        if (checkIfEntryOrExtern(line) == 1) {
            entryStep(line, head);
            stringCopy(line, iterator(line, inp, errors));
            continue;
        }
        stringCopy(line, iterator(line, inp, errors));
    }
    for (; i < *IC; i++) {
        if (mCode[i].labelUsageSource != NULL) {
            fillBlanks(tempNode, mCode, i, mCode[i].labelUsageSource);
            tempNode = head;
        }
        if (mCode[i].labelUsageDest != NULL) {
            fillBlanks(tempNode, mCode, i, mCode[i].labelUsageDest);
            tempNode = head;
        }
    }

    printf("\n===>>>>>> Second pass finished with %d errors\n", *errors);
    if (*IC > MAX_COMMANDS) {
        *errors += 1;
        printError("Max commands has exceeded limit");
    }
    if (*errors == 0) {
        fclose(inp);
        createOutPutFiles(mCode, head, outPutFileName, *dataCounter, *IC);
    } else {
        fclose(inp);
        printError("due to errors not continuing with flow on current file, continue with next file...\n");
    }
    freeMallocsFromPasses(mCode, head, *IC);
    return TRUE;
}
