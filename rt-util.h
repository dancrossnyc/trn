/* rt-util.h
*/
/* This software is copyrighted as detailed in the LICENSE file. */


extern char spin_char;          /* char to put back when we're done spinning */
extern long spin_estimate;      /* best guess of how much work there is */
extern long spin_todo;          /* the max word to do (might decrease) */
extern int  spin_count;         /* counter for when to spin */
extern int  spin_marks;         /* how many bargraph marks we want */

#define SPIN_OFF        0
#define SPIN_POP        1
#define SPIN_FOREGROUND 2
#define SPIN_BACKGROUND 3
#define SPIN_BARGRAPH   4

extern bool performed_article_loop;

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

char* extract_name (char*);
char* compress_name (char*,int);
char* compress_address (char*,int);
char* compress_from (char*,int);
char* compress_date (ARTICLE*,int);
bool subject_has_Re (char*,char**);
char* compress_subj (ARTICLE*,int);
void setspin (int);
void spin (int);
bool inbackground (void);
void perform_status_init (long);
void perform_status (long,int);
int perform_status_end (long,char*);
