/* bits.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


extern int dmcount;

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void bits_init (void);
void rc_to_bits (void);
bool set_firstart (char*);
void bits_to_rc (void);
void find_existing_articles (void);
void onemore (ARTICLE*);
void oneless (ARTICLE*);
void oneless_artnum (ART_NUM);
void onemissing (ARTICLE*);
void unmark_as_read (ARTICLE*);
void set_read (ARTICLE*);
void delay_unmark (ARTICLE*);
void mark_as_read (ARTICLE*);
void mark_missing_articles (void);
void check_first (ART_NUM);
void yankback (void);
int chase_xrefs (bool_int);
