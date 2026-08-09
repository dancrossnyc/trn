/*
 * This software is copyrighted as detailed in the LICENSE file.
 */

#pragma once

#define NGS_ABORT 0
#define NGS_FOUND 1
#define NGS_INTR 2
#define NGS_NOTFOUND 3
#define NGS_ERROR 4
#define NGS_DONE 5

extern bool ng_doempty;         /* search empty newsgroups? */

void ngsrch_init(void);
int ng_search(char *, int);
bool ng_wanted(NGDATA *);
const char *ng_comp(COMPEX *, char *, bool);
