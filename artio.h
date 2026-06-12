/* artio.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


static const int WRAPPED_NL = '\003';
static inline bool
AT_NL(int c)
{
    return c == '\n' || c == WRAPPED_NL;
}

extern ART_POS artpos;		/* byte position in article file */

extern ART_LINE artline;	/* current line number in article file */
extern FILE* artfp;		/* current article file pointer */
extern ART_NUM openart;		/* the article number we have open */

extern char* artbuf;
extern long artbuf_size;
extern long artbuf_pos;
extern long artbuf_seek;
extern long artbuf_len;

extern char wrapped_nl;
extern char* linkartname;	/* real name of article for Eunice */

#define WRAPPED_NL  '\003'
#define AT_NL(c) ((c) == '\n' || (c) == WRAPPED_NL)

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void artio_init (void);
FILE* artopen (ART_NUM,ART_POS);
void artclose (void);
int seekart (ART_POS);
ART_POS tellart (void);
char* readart (char*,int);
void clear_artbuf (void);
int seekartbuf (ART_POS);
char* readartbuf (bool_int);
