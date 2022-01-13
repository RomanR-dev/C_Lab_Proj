#include "definitions.h"
#include "utils.h"
#include "parsers.h"


char * concatenate(char ** toConCat, int max){
    char * line = malloc(80);
    switch (max){
        case 0:
            strcpy(line, toConCat[0]);
            break;
        case 1:
            strcpy(line, toConCat[0]);
            strcat(line, " ");
            strcat(line, toConCat[1]);
            break;
        case 2:
            strcpy(line, toConCat[0]);
            strcat(line, " ");
            strcat(line, toConCat[1]);
            strcat(line, ", ");
            strcat(line, toConCat[2]);
            break;
    }
    return line;
}


void printWrittenLine(int opCount, char ** parsedLine){
    int i = 0;
    for (; i <= opCount; i++) {
        printf("%s%s", parsedLine[i], i < opCount ? " " : "");
    }
}


bool lineSplitterFuncAndConcatenate(char ** parsedLine, int opCount, char * line, int errors) {
    parsedLine = chooseParser(line, &errors); /* parse line to get command and operands */
    if (!parsedLine) return FALSE;
    opCount = getOperandsCount(parsedLine[0], &errors); /* check how many operands per command is defined */
    if (opCount == -1) return FALSE;

    printWrittenLine(opCount, parsedLine); /* for debug purposes */
    strcpy(line, concatenate(parsedLine, opCount)); /* concat split line and copy it to the line str */
    return TRUE;
}


bool macroWriter(char * line, macroTable * table, FILE * outP){
    char * currLine = malloc(strlen(line));
    int macroCounter = 0;
    int i = 0;
    stringCopy(currLine, line);
    while (table[macroCounter].set == 1){
        if (strstr(currLine, table[macroCounter].name)){
            /*write macro*/
            for (;i<table[macroCounter].numOfCmnds; i++){
                fputs(table[macroCounter].cmnds[i], outP);
            }
            return TRUE;
        }
        macroCounter++;
    }
    return FALSE;
}


char ** firstPass(int argc, char ** argv ,int * newArgc){
    int errors = 0;
    int outputFileCounter = 1;
    int inputFileCounter = 1;
    int foundMacro = 0;
    bool writeResult;
    char line[80];
    char * outPutFileName = malloc(1);
    char ** filesForSecondPass = malloc(sizeof(char));
    macroTable * table = malloc(sizeof(macroTable));
    size_t filesForSecondPassSize = 0;
    FILE * outP;
    FILE * inp;


    while (inputFileCounter < argc) { /* iterate over files from argv .as */
        if ((inp = inputFileInit(argv,  inp, &inputFileCounter)) == NULL) continue;
        else outP = outputFileInit(outP, outPutFileName, &outputFileCounter);


        while ((fgets(line, 80, inp)) != NULL) { /* read from .as file and save macro data */
            if (strlen(line) > 80) {
                errors++;
                printf("line to long\n");
                continue;
            }
            if (line[0] == '\n') continue;
            if (line[0] == ';') continue;
            lstrip(line);
            /* macro finder  */
            if (strstr(line, "macro")) {
                foundMacro = 1;
                char ** tempLine = malloc(1);
                char * macroName = malloc(74);
                int counter = 0;
                strtok(line, " ");
                stringCopy(macroName, strtok(NULL,":"));
                while ((fgets(line, 80, inp)) != NULL) { /* read from .as file and save macro data */
                    if (strlen(line) > 80) {
                        errors++;
                        printf("line to long\n");
                        continue;
                    }
                    if (strstr(line, "endm")){
                        table = addMacro(table, macroName, tempLine, counter);
                        foundMacro = 0;
                        break;
                    }
                    lstrip(line);
                    tempLine[counter] = malloc(sizeof(char) * strlen(line));
                    stringCopy(tempLine[counter], line);
                    counter++;
                }
                if (foundMacro == 1){
                    errors++;
                }
                continue;
            }
            /* if call to macro check if macro exists if no error if so fputs*/
            writeResult = macroWriter(line, table, outP);
            if (!writeResult) fputs(line, outP);

        }

        fclose(outP); fclose(inp);
        printf("First pass finished: %s, errors: %d\n", argv[inputFileCounter-1], errors);
        if (errors == 0){
            filesForSecondPassSize = sizeof(char) * strlen(outPutFileName);
            filesForSecondPass[*newArgc] = malloc(filesForSecondPassSize);
            stringCopy(filesForSecondPass[*newArgc], outPutFileName);
            *newArgc += 1;
        }
        printf("============================================================\n");
    }
    return filesForSecondPass;
}

char ** secondPass(){
    int opCount;
    char ** parsedLine;


    return parsedLine;
}


int main(int argc, char ** argv){
    char ** newArgv;
    int newArgc = 0;
    newArgv = firstPass(argc, argv, &newArgc); /* first pass returns list of .am file names, updates amount in newArgc*/

    if (newArgc > 0) {
        int i = 0;
        printf("\n== report: newArgc: %d", newArgc);
        for (;i<newArgc;i++) {
            printf("\n== report: check files: %s", newArgv[i]);
        }
    }


//    free(newArgv);
    return 0;
}