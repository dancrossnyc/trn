/* ng.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


extern ART_NUM art;		/* current or prospective article # */

extern ART_NUM recent_art;	/* previous article # for '-' command */
extern ART_NUM curr_art;	/* current article # */
extern ARTICLE* recent_artp;	/* article_ptr equivilents */
extern ARTICLE* curr_artp;
extern ARTICLE* artp	;	/* the article ptr we use when art is 0 */

extern int checkcount;		/* how many articles have we read */
				/*   in the current newsgroup since */
				/*   the last checkpoint? */
extern int docheckwhen;		/* how often to do checkpoint */

extern char* subjline;		/* what format to use for '=' */
#ifdef MAILCALL
extern int mailcount;		/* check for mail when 0 mod 10 */
#endif
extern char* mailcall;
extern bool forcelast;		/* ought we show "End of newsgroup"? */
extern bool forcegrow;		/* do we want to recalculate size */
				/*   of newsgroup, e.g. after posting? */

#define NG_ERROR -1
#define NG_NORM 0
#define NG_ASK 1
#define NG_MINUS 2
#define NG_SELPRIOR 3
#define NG_SELNEXT 4
#define NG_NOSERVER 5
#define NG_NEXT 6
#define NG_GO_ARTICLE 7

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void ng_init (void);
int do_newsgroup (char*);
int art_switch (void);
#ifdef MAILCALL
void setmail (bool);
#endif
void setdfltcmd (void);
char ask_catchup (void);
bool output_subject (char*,int);
char ask_memorize (int);
