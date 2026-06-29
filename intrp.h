/* intrp.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


extern char* origdir;		/* cwd when rn invoked */
extern char* hostname;		/* host name to match local postings */
extern char* headname;
extern int perform_cnt;

#ifdef NEWS_ADMIN
extern char newsadmin[];	/* news administrator */
extern int newsuid;
#endif

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void intrp_init (char*,int);
char* dointerp (char*,int,char*,char*,char*);
char* interp_backslash (char*,char*);
char* interp (char*,int,char*);
char* interpsearch (char*,int,char*,char*);
void normalize_refs (char*);
