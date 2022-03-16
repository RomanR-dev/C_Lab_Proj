#include "misc/definitions.h"
#include "misc/utils.h"
#include "passes/pre_assembler.h"
#include "passes/first_pass.h"

/**
 * main wrapper function that parses the user inputs and executes the runs
 * @param argc
 * @param argv
 */
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
    printf("Received %d files, starting assembler process.\n\n", argc-1);
    while (inputFileCounter < argc) { /* iterate over files from argv .as */
        errors = 0;
        lineNum = 1;
        memset(line, '0', 4);
        if ((inp = inputFileInit(argv, inp, &inputFileCounter)) == NULL) {
            printError("file not found, skipping to next");
            continue;
        } else outP = outputFileInit(outP, outPutFileName, argv[inputFileCounter - 1]);

        while (!(strstr(line, "NULL"))) {
            stringCopy(line, iterator(line, inp, &errors));
            if (strstr(line, "NULL")) break;
            preAssembler(line, &errors, inp, outP, table); /* macro handler */
        }
        free(table);
        table = malloc(sizeof(macroTable));
        printf("===>>>>>> Pre assembler finished: Errors: %d\n", errors);
        if (errors == 0) {
            fclose(inp);
            lineNum = 1;
            firstPass(line, outP, &errors, outPutFileName);
        } else {
            fclose(outP);
            fclose(inp);
            remove(outPutFileName);
            printError("due to errors not continuing with flow on current file, continue with next file...");
        }
        printf("===>>>>>> Finished file: %s <<<<<<===\n", outPutFileName);
        printf("======================================================================\n\n");
    }
    free(table);
    printf("Finished processing - %d file(s) that were received.\n", argc-1);
}

int main(int argc, char **argv) {
    mainRunner(argc, argv);
    return 0;
}
