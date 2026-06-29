/* only.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#ifndef NBRA
#include "search.h"
#endif

extern char* ngtodo[MAXNGTODO];		/* restrictions in effect */
extern COMPEX* compextodo[MAXNGTODO];	/* restrictions in compiled form */

extern int maxngtodo;		/*  0 => no restrictions */
					/* >0 => # of entries in ngtodo */

extern char empty_only_char;

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void only_init (void);
void setngtodo (char*);
bool inlist (char*);
void end_only (void);
void push_only (void);
void pop_only (void);
