/* rt-process.h
*/
/* This software is copyrighted as detailed in the LICENSE file. */


/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

ARTICLE* allocate_article (ART_NUM);
int msgid_cmp (const void *, size_t, HASHDATUM);
bool valid_article (ARTICLE*);
ARTICLE* get_article (char*);
void thread_article (ARTICLE*,char*);
void rover_thread (ARTICLE*,char*);
void link_child (ARTICLE*);
void merge_threads (SUBJECT*,SUBJECT*);
