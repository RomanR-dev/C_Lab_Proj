#include "definitions.h"

/* utils */
void lstrip(char * l){
    int i = 0;
    int j = 0;
    while (l[i] == ' ') i++;
    while (l[i] != '\0'){
        l[j] = l[i];
        i++;
        j++;
    }
    l[j] = '\0';
}

void stringCopy(char * dest, char * src){
    int len = strlen(src);
    int i = 0;
    for (;i<len;i++){
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

void delay(int time){
    int i = 0;
    for (;i<time;i++);
}

FILE * openFile(char * fileName, FILE * inp){
    char * error = malloc(38 + strlen(fileName));
    char * filePath = malloc(15 + strlen(fileName));
    sprintf(error, "file: %s.as, returned error",fileName);
    sprintf(filePath, "../tester/%s.as", fileName);
    printf("Opening: %s...\n", filePath);
    inp = fopen(filePath, "r");
    if (inp == NULL){
        perror(error);
        return NULL;
    }
    else {
        return inp;
    }
}

FILE * inputFileInit(char ** argv, FILE * inp, int * inputFileCounter){
    /* file check and open if exists */
    inp = openFile(argv[*inputFileCounter], inp);
    if (inp == NULL){
        printf("============================================================\n");
        *inputFileCounter += 1;
        return NULL;
    }
    *inputFileCounter += 1;
    return inp;
}

FILE * outputFileInit(FILE * outP, char * outPutFileName, int * outputFileCounter){
    sprintf(outPutFileName, "../tester/output%d.am", *outputFileCounter);
    printf("Creating output file: %s\n", outPutFileName);
    outP = fopen(outPutFileName, "w+");
    *outputFileCounter += 1;
    return outP;
}