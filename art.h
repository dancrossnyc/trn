/* art.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


/* do_article() return values */

#define DA_NORM 0
#define DA_RAISE 1
#define DA_CLEAN 2
#define DA_TOEND 3

extern ART_LINE highlight;	/* next line to be highlighted */
extern ART_LINE first_view;
extern ART_POS raw_artsize;	/* size in bytes of raw article */
extern ART_POS artsize;		/* size in bytes of article */
extern char art_line[LBUFLEN];	/* place for article lines */

extern int gline;
extern ART_POS innersearch;	/* artpos of end of line we want to visit */
extern ART_LINE innerlight;	/* highlight position for innersearch or 0 */
extern char hide_everything;	/* if set, do not write page now, */
				/* ...but execute char when done with page */

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void art_init (void);
int do_article (void);
int maybe_set_color (char*,bool_int);
int page_switch (void);
bool innermore (void);
void pager_mouse (int,int,int,int,int,int);
