/* search.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */

#pragma once

struct compex {
    uint64_t array[40];
};

void init_compex(COMPEX *);
void free_compex(COMPEX *);
char *getbracket(COMPEX *, int);
char *compile(COMPEX *, char *, int, int);
char *execute(COMPEX *, char *);
