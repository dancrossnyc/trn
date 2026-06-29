/* head.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#define HEAD_FIRST SOME_LINE

/* types of header lines (if only C really believed in enums)
 * (These must stay in alphabetic order at least in the first letter.
 * Within each letter it helps to arrange in increasing likelihood.)
 */

#define PAST_HEADER	0			/* body */
#define SHOWN_LINE	(PAST_HEADER+1)		/* unrecognized but shown */
#define HIDDEN_LINE	(SHOWN_LINE+1)		/* unrecognized but hidden */
#define CUSTOM_LINE	(HIDDEN_LINE+1)		/* to isolate a custom line */
#define SOME_LINE	(CUSTOM_LINE+1)		/* default for unrecognized */
#define AUTHOR_LINE	(SOME_LINE+1)		/* Author */
#define BYTES_LINE	(AUTHOR_LINE+1)		/* Bytes */
#define CONTNAME_LINE	(BYTES_LINE+1)		/* Content-Name */
#define CONTDISP_LINE	(CONTNAME_LINE+1)	/* Content-Disposition */
#define CONTLEN_LINE	(CONTDISP_LINE+1)	/* Content-Length */
#define CONTXFER_LINE	(CONTLEN_LINE+1)	/* Content-Transfer-Encoding */
#define CONTTYPE_LINE	(CONTXFER_LINE+1)	/* Content-Type */
#define DIST_LINE	(CONTTYPE_LINE+1)	/* distribution */
#define DATE_LINE	(DIST_LINE+1)		/* date */
#define EXPIR_LINE	(DATE_LINE+1)		/* expires */
#define FOLLOW_LINE	(EXPIR_LINE+1)		/* followup-to */
#define FROM_LINE	(FOLLOW_LINE+1)		/* from */
#define INREPLY_LINE	(FROM_LINE+1)		/* in-reply-to */
#define KEYW_LINE	(INREPLY_LINE+1)	/* keywords */
#define LINES_LINE	(KEYW_LINE+1)		/* lines */
#define MIMEVER_LINE	(LINES_LINE+1)		/* mime-version */
#define MSGID_LINE	(MIMEVER_LINE+1)	/* message-id */
#define NGS_LINE	(MSGID_LINE+1)		/* newsgroups */
#define PATH_LINE	(NGS_LINE+1)		/* path */
#define RVER_LINE	(PATH_LINE+1)		/* relay-version */
#define REPLY_LINE	(RVER_LINE+1)		/* reply-to */
#define REFS_LINE	(REPLY_LINE+1)		/* references */
#define SUMRY_LINE	(REFS_LINE+1)		/* summary */
#define SUBJ_LINE	(SUMRY_LINE+1)		/* subject */
#define XREF_LINE	(SUBJ_LINE+1)		/* xref */
#define HEAD_LAST	(XREF_LINE+1)		/* total # of headers */

struct headtype {
    char* name;			/* header line identifier */
    ART_POS minpos;		/* pointer to beginning of line in article */
    ART_POS maxpos;		/* pointer to end of line in article */
    char length;		/* the header's string length */
    char flags;			/* the header's flags */
};

struct user_headtype {
    char* name;			/* user-defined headers */
    char length;		/* the header's string length */
    char flags;			/* the header's flags */
};

#define HT_HIDE	    0x01	/* hide this line */
#define HT_MAGIC    0x02	/* do any special processing on this line */
#define HT_CACHED   0x04	/* this information is cached article data */
#define HT_DEFHIDE  0x08	/* hidden by default */
#define HT_DEFMAGIC 0x10	/* magic by default */
#define HT_MAGICOK  0x20	/* magic even possible for line */

/* This array must stay in the same order as the list above */

#define HIDDEN    (HT_HIDE|HT_DEFHIDE)
#define MAGIC_ON  (HT_MAGICOK|HT_MAGIC|HT_DEFMAGIC)
#define MAGIC_OFF (HT_MAGICOK)

#ifdef DBM_XREFS
#define XREF_CACHED 0
#define NGS_CACHED  HT_CACHED
#else
#define XREF_CACHED HT_CACHED
#define NGS_CACHED  0
#endif
#ifdef USE_FILTER
#define FILT_CACHED HT_CACHED
#else
#define FILT_CACHED 0
#endif

extern struct headtype htype[HEAD_LAST];
extern struct user_headtype* user_htype;
extern short user_htypeix[26];
extern int user_htype_cnt;
extern int user_htype_max;

extern ART_NUM parsed_art;	/* the article number we've parsed */
extern ARTICLE* parsed_artp;	/* the article ptr we've parsed */
extern int in_header;		/* are we decoding the header? */
extern char* headbuf;
extern long headbuf_size;

#define PREFETCH_SIZE 5

#define fetchsubj(artnum,copy) prefetchlines(artnum,SUBJ_LINE,copy)
#define fetchfrom(artnum,copy) prefetchlines(artnum,FROM_LINE,copy)
#ifdef DBM_XREFS
#define fetchxref(artnum,copy) prefetchlines(artnum,NGS_LINE,copy)
#else
#define fetchxref(artnum,copy) prefetchlines(artnum,XREF_LINE,copy)
#endif

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void head_init (void);
#ifdef DEBUG
void dumpheader (char*);
#endif
int set_line_type (char*,char*);
int get_header_num (char*);
void start_header (ART_NUM);
void end_header_line (void);
bool parseline (char*,int,int);
void end_header (void);
bool parseheader (ART_NUM);
char* fetchlines (ART_NUM,int);
#ifdef SCAN
char* mp_fetchlines (ART_NUM,int,int);
#endif
char* prefetchlines (ART_NUM,int,bool);
