#include "misc/definitions.h"
#include "misc/utils.h"
#include "misc/parsers.h"
#include "pre_assembler.h"
#include "first_pass.h"


void freeMachineCodes(machineCode *mCode) {
    int counter = 100;
    for (; counter < MAX_COMMANDS; counter++) {
        if (mCode[counter].set != '\0') {
            if (mCode[counter].set == 'd') free(mCode[counter].word.data);
            else if (mCode[counter].set == 'c') free(mCode[counter].word.code);
        }
        counter++;
    }
}

void mainRunner(int argc, char **argv) {
    int errors = 0;
    int inputFileCounter = 1;
    char line[MAX_LENGTH];
    char *outPutFileName = malloc(sizeof(char));
    checkMalloc(outPutFileName);
    macroTable *table = malloc(sizeof(macroTable));
    checkMalloc(table);
    FILE *outP;
    FILE *inp;

    while (inputFileCounter < argc) { /* iterate over files from argv .as */
        if ((inp = inputFileInit(argv, inp, &inputFileCounter)) == NULL) {
            printf("file not found, skipping to next");
            continue;
        } else outP = outputFileInit(outP, outPutFileName, argv[inputFileCounter - 1]);

        while (!(strstr(line, "NULL"))) {
            stringCopy(line, iterator(line, inp, &errors));
            if (strstr(line, "NULL")) break;
            preAssembler(line, &errors, inp, outP, table); /* macro handler */
        }
        printf("pre assembler finished: %s, errors: %d", argv[inputFileCounter - 1], errors);
        printf("\n============================================================\n");
        inputFileCounter++;
        if (errors == 0) {
            fclose(inp);
            firstPass(line, outP, &errors, outPutFileName);
        } else {
            fclose(outP);
            fclose(inp);
            printf("due to errors not continuing with flow on current file, continue with next file...");
        }
    }
    free(table);
    fclose(inp);
    fclose(outP);
}


int main(int argc, char **argv) {
    mainRunner(argc, argv); /* first pass returns list of .am file names, updates amount in newArgc*/






//    free(newArgv);
    return 0;
}