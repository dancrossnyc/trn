/* ngstuff.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#define NN_NORM 0
#define NN_INP 1
#define NN_REREAD 2
#define NN_ASK 3

EXT bool one_command INIT(FALSE);	/* no ':' processing in perform() */

/* CAA: given the new and complex universal/help possibilities,
 *      the following interlock variable may save some trouble.
 *      (if TRUE, we are currently processing options)
 */
EXT bool option_sel_ilock INIT(FALSE);

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void ngstuff_init (void);
int escapade (void);
int switcheroo (void);
int numnum (void);
int thread_perform (void);
int perform (char*,int);
int ngsel_perform (void);
int ng_perform (char*,int);
int addgrp_sel_perform (void);
int addgrp_perform (ADDGROUP*,char*,int);
