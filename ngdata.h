/* ngdata.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


struct ngdata {
    NGDATA* prev;
    NGDATA* next;
    NEWSRC* rc;		/* which rc is this line from? */
    char* rcline;	/* pointer to group's .newsrc line */
    ART_NUM abs1st;	/* 1st real article in newsgroup */
    ART_NUM ngmax;	/* high message num for the group */
    ART_UNREAD toread;	/* number of articles to be read in newsgroup */
			/* < 0 is invalid or unsubscribed newsgroup */
    NG_NUM num;		/* a possible sort order for this group */
    int numoffset;	/* offset from rcline to numbers on line */
    char subscribechar;	/* holds the character : or ! while spot is \0 */
    char flags;  	/* flags for each group */
};

extern LIST* ngdata_list; /* a list of NGDATA */
extern int ngdata_cnt;
extern NG_NUM newsgroup_cnt; /* all newsgroups in our current newsrc(s) */
extern NG_NUM newsgroup_toread;
extern ART_UNREAD ng_min_toread; /* == TR_ONE or TR_NONE */

extern NGDATA* first_ng;
extern NGDATA* last_ng;
extern NGDATA* ngptr;	/* current newsgroup data ptr */

extern NGDATA* current_ng;/* stable current newsgroup so we can ditz with ngptr */
extern NGDATA* recent_ng; /* the prior newsgroup we visited */
extern NGDATA* starthere; /* set to the first newsgroup with unread news on startup */

#define ngdata_ptr(ngnum) ((NGDATA*)listnum2listitem(ngdata_list,(long)(ngnum)))
/*#define ngdata_num(ngptr) listitem2listnum(ngdata_list,(char*)ngptr)*/

extern NGDATA* sel_page_np;
extern NGDATA* sel_next_np;

extern ART_NUM absfirst;	/* 1st real article in current newsgroup */
extern ART_NUM firstart;	/* minimum unread article number in newsgroup */
extern ART_NUM lastart;	/* maximum article number in newsgroup */
extern ART_UNREAD missing_count;	/* for reports on missing articles */

extern char* moderated;
extern char* redirected;
extern bool ThreadedGroup;

/* CAA goto-newsgroup extensions */
extern NGDATA* ng_go_ngptr;
extern ART_NUM ng_go_artnum;
extern char* ng_go_msgid;

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void ngdata_init (void);
void set_ng (NGDATA*);
int access_ng (void);
void chdir_newsdir (void);
void grow_ng (ART_NUM);
void sort_newsgroups (void);
void ng_skip (void);
ART_NUM getngsize (NGDATA*);
