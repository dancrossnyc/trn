/* respond.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


extern char* savedest;          /* value of %b */
extern char* extractdest;       /* value of %E */
extern char* extractprog;       /* value of %e */
extern ART_POS savefrom;                /* value of %B */

#define SAVE_ABORT 0
#define SAVE_DONE 1

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void respond_init (void);
int save_article (void);
int view_article (void);
int cancel_article (void);
int supersede_article (void);
void reply (void);
void forward (void);
void followup (void);
int invoke (char*,char*);
