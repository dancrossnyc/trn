/* rt-wumpus.h
*/
/* This software is copyrighted as detailed in the LICENSE file. */


/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void init_tree (void);
ARTICLE* get_tree_artp (int,int);
int tree_puts (char*,ART_LINE,int);
int finish_tree (ART_LINE);
void entire_tree (ARTICLE*);
char thread_letter (ARTICLE*);
