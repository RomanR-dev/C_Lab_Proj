#ifndef _definitions_c
#define _definitions_c
#include "definitions.h"

/* data consts */
func functions[] = {
        {"mov",  0,  0,  2},
        {"cmp",  1,  0,  2},
        {"add",  2,  10, 2},
        {"sub",  2,  11, 2},
        {"lea",  4,  0,  2},

        {"clr",  5,  10, 1},
        {"not",  5,  11, 1},
        {"inc",  5,  12, 1},
        {"dec",  5,  13, 1},
        {"jmp",  9,  10, 1},
        {"bne",  9,  11, 1},
        {"jsr",  9,  12, 1},
        {"red",  12, 0,  1},
        {"prn",  13, 0,  1},

        {"rts",  14, 0,  0},
        {"stop", 15, 0,  0}
};

#endif
