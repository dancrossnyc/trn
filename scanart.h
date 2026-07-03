/* This file Copyright 1992 by Clifford A. Adams */
/* scanart.h
 *
 * Interface to rest of [t]rn
 */

/* return codes for sa_main */
        /* read article pointed to by art (always) */
#define SA_READ (-7)
        /* quit, and return to previous selector (backtick) */
#define SA_QUIT_SEL (-6)
        /* go to and enter prior newsgroup */
#define SA_PRIOR (-5)
        /* go to and enter next newsgroup */
#define SA_NEXT (-4)
        /* Fake a command (buf and art already set up) */
#define SA_FAKE (-3)
        /* error, quit out one level */
#define SA_ERR (-2)
        /* quit out one level and clean up... */
#define SA_QUIT (-1)
        /* do the normal thing (usually read article pointed to by art) */
#define SA_NORM 0

/* per-entry data */
struct sa_entrydata {
    ART_NUM artnum;
    long subj_thread_num;
    char sa_flags;              /* status bitmap (marked, select, etc...) */
};

extern SA_ENTRYDATA* sa_ents;
extern int sa_num_ents;
extern int sa_ents_alloc;

extern bool sa_initialized;     /* Have we initialized? */
extern bool sa_never_initialized; /* Have we ever initialized? */

/* note: sa_in should be checked for returning to SA */
extern bool sa_in;              /* Are we "in" SA? */

extern bool sa_go;              /* go to sa.  Do not pass GO (:-) */
extern bool sa_go_explicit;     /* want to bypass read-next-marked */

extern bool sa_context_init;    /* has context been initialized? */

/* used to pass an article number to read soon */
extern ART_NUM sa_art;

/* reimplement later */
/* select threads from TRN thread selector */
extern bool sa_do_selthreads;

/* true if read articles are eligible */
/* in scanart.h for world-visibilty */
extern bool sa_mode_read_elig;

/* Options */
/* Display order variable:
 *
 * 1: Arrival order
 * 2: Descending score
 */
extern int sa_mode_order;

/* if true, don't move the cursor after marking or selecting articles */
extern bool sa_mark_stay;

/* if true, re-"fold" after an un-zoom operation. */
/* This flag is useful for very slow terminals */
extern bool sa_unzoomrefold;

/* true if in "fold" mode */
extern bool sa_mode_fold;

/* Follow threads by default? */
extern bool sa_follow;

/* Options: what to display */
extern bool sa_mode_desc_artnum;        /* show art#s */
extern bool sa_mode_desc_author;        /* show author */
extern bool sa_mode_desc_score; /* show score */
/* flags to determine whether to display various things */
extern bool sa_mode_desc_threadcount;
extern bool sa_mode_desc_subject;
extern bool sa_mode_desc_summary;
extern bool sa_mode_desc_keyw;

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

int sa_main (void);
void sa_grow (ART_NUM,ART_NUM);
void sa_cleanup (void);
