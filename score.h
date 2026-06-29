/* This file Copyright 1992 by Clifford A. Adams */
/* score.h
 *
 */

/* RETHINK LOWSCORE: (especially for 16-bit scores?) */
/* score given to unavailable articles */
#define LOWSCORE (-999999)

/* specific scoreflag meanings:  (note: bad placement, but where else?) */
/* author has a score (match on FROM: line) */
#define SFLAG_AUTHOR 1
/* if true, the article has been scored */
#define SFLAG_SCORED 16
#define SCORED(a) (article_ptr(a)->scoreflags & SFLAG_SCORED)

extern  bool kill_thresh_active;
extern  int kill_thresh;   /* KILL articles at or below this score */

extern  ART_NUM sc_fill_max;	   /* maximum art# scored by fill-routine */
extern  bool sc_fill_read; /* true if also scoring read arts... */

/* has score been initialized (are we "in" scoring?) */
extern  bool sc_initialized;

/* are we currently scoring an article (prevents loops) */
extern  bool sc_scoring;

/* changes order of sorting (artnum comparison) when scores are equal */
extern  bool score_newfirst;

/* if nice background available, use it */
extern  bool sc_mode_nicebg;

/* If true, save the scores for this group on exit. */
extern  bool sc_savescores;

/* If true, delay initialization of scoring until explicitly required */
extern  bool sc_delay;

extern  bool sc_rescoring;	/* are we rescoring now? */

extern  bool sc_do_spin;	/* actually do the score spinner */

extern  bool sc_sf_delay;	/* if true, delay loading rule files */
extern  bool sc_sf_force_init;	/* If true, always sf_init() */

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void sc_init (bool);
void sc_cleanup (void);
void sc_set_score (ART_NUM,int);
void sc_score_art_basic (ART_NUM);
int sc_score_art (ART_NUM,bool);
void sc_fill_scorelist (ART_NUM,ART_NUM);
void sc_lookahead (bool,bool);
int sc_percent_scored (void);
void sc_rescore_arts (void);
void sc_append (char*);
void sc_rescore (void);
void sc_score_cmd (char*);
void sc_kill_threshold (int);
