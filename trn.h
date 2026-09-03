/* trn.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


extern char* ngname;       /* name of current newsgroup */
extern size_t ngnlen;      /* current malloced size of ngname */
extern size_t ngname_len;  /* length of current ngname */
extern char* ngdir;        /* same thing in directory name form */
extern size_t ngdlen;      /* current malloced size of ngdir */

#define ING_NORM        0
#define ING_ASK         1
#define ING_INPUT       2
#define ING_ERASE       3
#define ING_QUIT        4
#define ING_ERROR       5
#define ING_SPECIAL     6
#define ING_BREAK       7
#define ING_RESTART     8
#define ING_NOSERVER    9
#define ING_DISPLAY     10
#define ING_MESSAGE     11

extern int ing_state;

#define INGS_CLEAN      0
#define INGS_DIRTY      1

extern bool  write_less;        /* write .newsrc less often */

extern char* auto_start_cmd;    /* command to auto-start with */
extern bool  auto_started;      /* have we auto-started? */

extern bool  is_strn;           /* Is this "strn", or trn/rn? */

extern char patchlevel[];

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void trn_init (void);
int main (int,char**);
void do_multirc (void);
int input_newsgroup (void);
void check_active_refetch (bool);
void trn_version (void);
void set_ngname (char*);
char* getngdir (char*);
