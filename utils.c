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