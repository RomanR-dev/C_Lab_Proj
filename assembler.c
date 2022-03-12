#include "misc/definitions.h"
#include "misc/utils.h"
#include "passes/pre_assembler.h"
#include "passes/first_pass.h"

void mainRunner(int argc, char **argv) {
    int errors = 0;
    int inputFileCounter = 1;
    char line[MAX_LENGTH];
    char *outPutFileName = malloc(sizeof(char));
    macroTable *table = malloc(sizeof(macroTable));
    FILE *outP;
    FILE *inp;
    checkMalloc(outPutFileName);
    checkMalloc(table);

    while (inputFileCounter < argc) { /* iterate over files from argv .as */
        memset(line, '0', 4);
        if ((inp = inputFileInit(argv, inp, &inputFileCounter)) == NULL) {
            printf("file not found, skipping to next");
            continue;
        } else outP = outputFileInit(outP, outPutFileName, argv[inputFileCounter - 1]);

        while (!(strstr(line, "NULL"))) {
            stringCopy(line, iterator(line, inp, &errors));
            if (strstr(line, "NULL")) break;
            preAssembler(line, &errors, inp, outP, table); /* macro handler */
        }
        printf("===>>>>>> Pre assembler finished: %s, errors: %d\n", argv[inputFileCounter - 1], errors);
        if (errors == 0) {
            fclose(inp);
            firstPass(line, outP, &errors, outPutFileName);
        } else {
            fclose(outP);
            fclose(inp);
            printf("due to errors not continuing with flow on current file, continue with next file...");
        }
        printf("\n===>>>>>> Finished file: %s <<<<<<===\n", outPutFileName);
        printf("======================================================================\n\n");
    }
    freemallocsMainRunner(table, inp, outP);
}

int main(int argc, char **argv) {
    mainRunner(argc, argv); /* first pass returns list of .am file names, updates amount in newArgc*/
    return 0;
}
