/* last.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


extern char* lastngname;        /* last newsgroup read */
extern long lasttime;           /* time last we ran */
extern long lastactsiz;         /* last known size of active file */
extern long lastnewtime;        /* time of last newgroup request */
extern long lastextranum;

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void last_init (void);
void readlast (void);
void writelast (void);
