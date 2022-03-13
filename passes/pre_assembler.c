#include "../misc/definitions.h"
#include "../misc/utils.h"
#include "../misc/parsers.h"

macroTable *addMacro(macroTable *table, char *name, char **cmd, int numOfCmds) {
    int i = 0;
    int j = 0;
    while (table[j].set == 1) {
        j++;
    }
    table = realloc(table, (sizeof(macroTable) * j) + sizeof(macroTable));
    table[j].set = 1;
    table[j].name = malloc(strlen(name));
    stringCopy(table[j].name, name);
    table[j].cmnds = malloc(1);
    while (numOfCmds > i) {
        table[j].cmnds[i] = malloc(strlen(cmd[i]));
        strcpy(table[j].cmnds[i], cmd[i]);
        i++;
    }
    table[j].numOfCmnds = numOfCmds;
    return table;
}

bool macroWriter(char *line, macroTable *table, FILE *outP) {
    int macroCounter = 0;
    int i = 0;
    while (table[macroCounter].set == 1) {
        if (strstr(line, table[macroCounter].name) || strstr(table[macroCounter].name, line)) {
            /*write macro*/
            for (; i < table[macroCounter].numOfCmnds; i++) {
                fputs(table[macroCounter].cmnds[i], outP);
            }
            return TRUE;
        }
        macroCounter++;
    }
    return FALSE;
}

void preAssembler(char *line, int *errors, FILE *inp, FILE *outP, macroTable *table) {
    bool endm = FALSE;
    bool writeResult = FALSE;
    /* macro finder  */
    if (strstr(line, "macro")) {
        char **tempLine = (char **) malloc(1);
        char *macroName = (char *) malloc(74);
        int counter = 0;
        checkMalloc(macroName);
        strtok(line, " ");
        stringCopy(macroName, strtok(NULL, ":"));
        if (macroName[strlen(macroName) - 1] == '\n') macroName[strlen(macroName) - 1] = '\0';
        while ((fgets(line, 80, inp)) != NULL) { /* read from .as file and save macro data */
            if (*line == '\n') continue;
            if (strlen(line) > 80) {
                errors += 1;
                printf("line to long\n");
                continue;
            }
            if (strstr(line, "endm")) {
                endm = TRUE;
                table = addMacro(table, macroName, tempLine, counter);
                break;
            }
            lstrip(line);
            tempLine[counter] = (char *) malloc(strlen(line) + 1);
            checkMalloc(tempLine[counter]);
            stringCopy(tempLine[counter], line);
            counter++;
            if (strstr(line, macroName)) {
                *errors += 1;
                printError(
                        "macro doesnt have a closure with 'endm' will not create am file and continue with process.");
                break;
            }
        }
    }
    /* if written macro continue to next line, else write the line as is to new file */
    writeResult = macroWriter(line, table, outP);
    if (!writeResult && endm == FALSE) {
        fprintf(outP, "%s", line);
    }
}