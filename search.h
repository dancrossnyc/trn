/* search.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */

#include <stdbool.h>

#pragma once

/*
 * A `struct compex` is an opaque structure that holds a compiled
 * regular expression and associated state.
 */
struct compex {
    uint64_t array[40];
};

void init_compex(COMPEX *);
void free_compex(COMPEX *);
char *getbracket(COMPEX *, size_t);
const char *compile(COMPEX *, const char *, bool);
const char *execute(COMPEX *, const char *);
