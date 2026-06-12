/* backpage.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */

/* things for doing the 'back page' command */

extern int varyfd;			/* virtual array file for storing  */
					/* file offsets */
extern ART_POS varybuf[VARYSIZE];	/* current window onto virtual array */

extern long oldoffset;			/* offset to block currently in window */

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void backpage_init (void);
ART_POS vrdary (ART_LINE);
void vwtary (ART_LINE,ART_POS);
