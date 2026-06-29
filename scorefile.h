/* This file Copyright 1992 by Clifford A. Adams */
/* scorefile.h
 *
 */

#define DEFAULT_SCOREDIR "%+/scores"

struct sf_entry {
    int head_type;	/* header # (see head.h) */
    int score;		/* score change */
    char* str1;		/* first string part */
    char* str2;		/* second string part */
    COMPEX* compex;	/* regular expression ptr */
    char flags;		/* 1: regex is valid
			 * 2: rule has been applied to the current article.
			 * 4: use faster rule checking  (later)
			 */
};
/* note that negative header #s are used to indicate special entries... */

extern int sf_num_entries;	/* # of entries */
extern SF_ENTRY* sf_entries;	/* array of entries */

#ifdef SCOREFILE_CACHE
/* for cached score rules */
struct sf_file {
    char* fname;
    int num_lines;
    int num_alloc;
    long line_on;
    char** lines;
};

extern SF_FILE *sf_files;
extern int sf_num_files;
#endif

extern char **sf_abbr;		/* abbreviations */

/* when true, the scoring routine prints lots of info... */
extern int sf_score_verbose;

extern bool sf_verbose;  /* if true print more stuff while loading */

/* if true, only header types that are cached are scored... */
extern bool cached_rescore;

/* if true, newauthor is active */
extern bool newauthor_active;
/* bonus score given to a new (unscored) author */
extern int newauthor;

/* if true, reply_score is active */
extern bool reply_active;
/* score amount added to an article reply */
extern int reply_score;

/* should we match by pattern? */
extern int sf_pattern_status;

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void sf_init (void);
void sf_clean (void);
void sf_grow (void);
int sf_check_extra_headers (char*);
void sf_add_extra_header (char*);
char* sf_get_extra_header (ART_NUM,int);
bool is_text_zero (char*);
char* sf_get_filename (int);
char* sf_cmd_fname (char*);
bool sf_do_command (char*,bool);
char* sf_freeform (char*,char*);
bool sf_do_line (char*,bool);
void sf_do_file (char*);
int score_match (char*,int);
int sf_score (ART_NUM);
char* sf_missing_score (char*);
void sf_append (char*);
char* sf_get_line (ART_NUM,int);
void sf_print_match (int);
void sf_exclude_file (char*);
void sf_edit_file (char*);
