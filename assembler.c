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

FILE * openFile(char * fileName){
    char * error = malloc(38 + strlen(fileName));
    char * filePath = malloc(15 + strlen(fileName));
    sprintf(error, "file: %s.as, returned error",fileName);
    sprintf(filePath, "../tester/%s.as", fileName);
    printf("Opening: %s...\n", filePath);
    FILE * inp = fopen(filePath, "r");
    if (inp == NULL){
        perror(error);
        return NULL;
    }
    else {
        return inp;
    }
}

void printWrittenLine(int opCount, char ** parsedLine){
    int i = 0;
    for (; i <= opCount; i++) {
        printf("%s%s", parsedLine[i], i < opCount ? " " : "");
    }
}


int lineSplitterFuncAndConcatenator(char ** parsedLine, int opCount, char * line, int errors){
    parsedLine = chooseParser(line, &errors); /* parse line to get command and operands */
    if (!parsedLine) return -1;
    opCount = getOperandsCount(parsedLine[0], &errors); /* check how many operands per command is defined */
    if (opCount == -1) return -1;

    printWrittenLine(opCount, parsedLine); /* for debug purposes */
    strcpy(line, concatenate(parsedLine, opCount)); /* concat split line and copy it to the line str */
    return 0;
}

char ** firstPass(int argc, char ** argv ,int * newArgc){
    int errors = 0;
    int outputFileCounter = 1;
    int inputFileCounter = 1;
    int foundMacro = 0;
    char line[80];
    char * outPutFileName = malloc(1);
    char ** filesForSecondPass = malloc(sizeof(char));
    macroTable * table = malloc(1);
    size_t filesForSecondPassSize = 0;
    FILE * outP;
    FILE * inp;


    while (inputFileCounter < argc) { /* iterate over files from argv .as */
//        sprintf(outPutFileName, "../tester/output%d.am", outputFileCounter);
//        printf("Creating output file: %s\n", outPutFileName);
//        outP = fopen(outPutFileName, "w+");
        inp = openFile(argv[inputFileCounter]);
        if (inp == NULL){
//            fclose(outP);
//            remove(outPutFileName);
            printf("============================================================\n");
            inputFileCounter++;
            continue;
        }
//        outputFileCounter++;
        inputFileCounter++;

        while ((fgets(line, 80, inp)) != NULL) { /* read from .as file and save macro data */
            if (strlen(line) > 80) {
                errors++;
                printf("line to long\n");
                continue;
            }
            lstrip(line);
            if (line[0] == ';') continue;
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
                        addMacro(table, macroName, tempLine, counter);
                        foundMacro = 0;
                        break;
//                        free(macroName); free(tempLine);
                    }
                    lstrip(line);
                    tempLine[counter] = malloc(sizeof(char) * strlen(line));
                    stringCopy(tempLine[counter], line);
                    counter++;
                }
                if (foundMacro == 1){
                    errors++;
                }
            }






//            fputs(line, outP);
        }
//        fclose(outP); fclose(inp);
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


    free(newArgv);
    return 0;
}