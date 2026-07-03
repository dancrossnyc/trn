/* This file Copyright 1993 by Clifford A. Adams */
/* scoresave.h
 *
 */

extern long sc_save_new;        /* new articles (unloaded) */
extern long sc_loaded_count;    /* how many articles were loaded? */

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void sc_sv_add (char*);
void sc_sv_delgroup (char*);
void sc_sv_getfile (void);
void sc_sv_savefile (void);
ART_NUM sc_sv_use_line (char*,ART_NUM);
ART_NUM sc_sv_make_line (ART_NUM);
void sc_load_scores (void);
void sc_save_scores (void);
