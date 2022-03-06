#ifndef _UTILS_
#define _UTILS_

/* utils */
void lstrip(char * l);

void stringCopy(char * dest, char * src);

FILE * openFile(char * fileName, FILE * inp);

FILE * inputFileInit(char ** argv, FILE * inp, int * inputFileCounter);

FILE * outputFileInit(FILE * outP, char * outPutFileName, char * inputName);

void delay(int time); /* not necessary */

#endif

