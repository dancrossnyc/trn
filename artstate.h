/* artstate.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


extern bool reread;             /* consider current art temporarily */
                                /* unread? */

extern bool do_fseek;           /* should we back up in article file? */

extern bool oldsubject;         /* not 1st art in subject thread */
extern ART_LINE topline;        /* top line of current screen */
extern bool do_hiding;          /* hide header lines with -h? */
extern bool is_mime;            /* process mime in an article? */
extern bool multimedia_mime;    /* images/audio to see/hear? */
extern bool rotate;             /* has rotation been requested? */
extern char* prompt;            /* pointer to current prompt */

extern char* firstline;         /* special first line? */
#ifdef CUSTOMLINES
extern char* hideline;          /* custom line hiding? */
extern char* pagestop;          /* custom page terminator? */
extern COMPEX hide_compex;
extern COMPEX page_compex;
#endif
