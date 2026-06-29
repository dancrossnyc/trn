/* artsrch.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#ifndef NBRA
#include "search.h"
#endif

#ifdef ARTSEARCH

#define SRCH_ABORT 0
#define SRCH_INTR 1
#define SRCH_FOUND 2
#define SRCH_NOTFOUND 3
#define SRCH_DONE 4
#define SRCH_SUBJDONE 5
#define SRCH_ERROR 6
#endif

extern char* lastpat;		/* last search pattern */
#ifdef ARTSEARCH
extern COMPEX sub_compex;	/* last compiled subject search */
extern COMPEX art_compex;	/* last compiled normal search */
extern COMPEX* bra_compex;	/* current compex with brackets */

#define ARTSCOPE_SUBJECT	0
#define ARTSCOPE_FROM		1
#define ARTSCOPE_ONEHDR		2
#define ARTSCOPE_HEAD		3
#define ARTSCOPE_BODY_NOSIG	4
#define ARTSCOPE_BODY		5
#define ARTSCOPE_ARTICLE	6

extern char scopestr[];
extern int art_howmuch;		/* search scope */
extern int art_srchhdr;		/* specific header number to search */
extern bool art_doread;		/* search read articles? */
#endif

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void artsrch_init (void);
#ifdef ARTSEARCH
int art_search (char*,int,int);
bool wanted (COMPEX*,ART_NUM,int);
#endif
