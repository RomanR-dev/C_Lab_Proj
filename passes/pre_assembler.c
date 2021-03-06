#include "../misc/definitions.h"
#include "../misc/utils.h"

/**
 * add macro to the macro's table during pre-assembler
 * @param table
 * @param name
 * @param cmd
 * @param numOfCmds
 * @return
 */
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

/**
 * write macro to .am file
 * @param line
 * @param table
 * @param outP
 * @return
 */
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

/**
 * process the initial .as file as is, and unpacks any defined and used macros
 * @param line
 * @param errors
 * @param inp
 * @param outP
 * @param table
 */
void preAssembler(char *line, int *errors, FILE *inp, FILE *outP, macroTable *table) {
    bool endm = FALSE;
    bool writeResult = FALSE;
    /* macro finder  */
    if (strstr(line, "macro")) {
        char **tempLine = (char **) malloc(1);
        char *macroName = (char *) malloc(40);
        int counter = 0;
        checkMalloc(macroName);
        strtok(line, " ");
        stringCopy(macroName, strtok(NULL, ":"));
        if (macroName[strlen(macroName) - 1] == '\n') macroName[strlen(macroName) - 1] = '\0';
        while ((fgets(line, 80, inp)) != NULL) { /* read from .as file and save macro data */
            lineNum++;
            if (*line == '\n') continue;
            if (strlen(line) > 80) {
                errors += 1;
                printf("%d --->line to long\n", lineNum);
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
            swapLastCharIfNewLine(line);
            if (strstr(line, macroName)) {
                *errors += 1;
                printf("--->macro doesnt have a closure with 'endm' (found macro call) will not create am file and continue with process.\n");
                break;
            }
        }
        if (endm == FALSE) {
            *errors += 1;
            printf("--->macro doesnt have a closure with 'endm' (end of code) will not create am file and continue with process.\n");
        }
    }
    /* if written macro continue to next line, else write the line as is to new file */
    writeResult = macroWriter(line, table, outP);
    if (!writeResult && endm == FALSE) {
        fprintf(outP, "%s", line);
    }
}
