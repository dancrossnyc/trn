/* search.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */

#include <stdbool.h>

#pragma once

struct compex {
    uint64_t array[40];
};

void init_compex(COMPEX *);
void free_compex(COMPEX *);
char *getbracket(COMPEX *, int);
const char *compile(COMPEX *, const char *, bool);
const char *execute(COMPEX *, const char *);
