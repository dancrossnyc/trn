/* init.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#define TCBUF_SIZE 1024

extern long our_pid;
/* default string for group entry */
#if 0
extern char *group_default;
#endif

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

bool initialize (int,char**);
void newsnews_check (void);
