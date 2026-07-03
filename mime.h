/* mime.h
 */

struct hblk {
    int     tnum;
    short   cnt;
    char    indent;
};

struct mime_sect {
    MIME_SECT* prev;
    char*   filename;
    char*   type_name;
    char*   type_params;
    char*   boundary;
    int     html_line_start;
    HBLK*   html_blks;
    short   type;
    short   encoding;
    short   part;
    short   total;
    short   boundary_len;
    short   flags;
    short   html;
    short   html_blkcnt;
};

#define MSF_INLINE      0x0001
#define MSF_ALTERNATIVE 0x0002
#define MSF_ALTERNADONE 0x0004

/* Only used with HTMLTEXT_MIME */
#define HF_IN_TAG       0x0001
#define HF_IN_COMMENT   0x0002
#define HF_IN_HIDING    0x0004
#define HF_IN_PRE       0x0008
#define HF_IN_DQUOTE    0x0010
#define HF_IN_SQUOTE    0x0020
#define HF_QUEUED_P     0x0040
#define HF_P_OK         0x0080
#define HF_QUEUED_NL    0x0100
#define HF_NL_OK        0x0200
#define HF_NEED_INDENT  0x0400
#define HF_SPACE_OK     0x0800
#define HF_COMPACT      0x1000

#define HTML_MAX_BLOCKS 256

#define TF_BLOCK        0x0001          /* This implies TF_HAS_CLOSE */
#define TF_HAS_CLOSE    0x0002
#define TF_NL           0x0004
#define TF_P            0x0008
#define TF_BR           0x0010
#define TF_LIST         0x0020
#define TF_HIDE         0x0040
#define TF_SPACE        0x0080
#define TF_TAB          0x0100

/* NOTE: This must match tagattr below */
#define TAG_BLOCKQUOTE  0
#define TAG_BR          (TAG_BLOCKQUOTE+1)
#define TAG_DIV         (TAG_BR+1)
#define TAG_HR          (TAG_DIV+1)
#define TAG_IMG         (TAG_HR+1)
#define TAG_LI          (TAG_IMG+1)
#define TAG_OL          (TAG_LI+1)
#define TAG_P           (TAG_OL+1)
#define TAG_PRE         (TAG_P+1)
#define TAG_SCRIPT      (TAG_PRE+1)
#define TAG_STYLE       (TAG_SCRIPT+1)
#define TAG_TD          (TAG_STYLE+1)
#define TAG_TH          (TAG_TD+1)
#define TAG_TR          (TAG_TH+1)
#define TAG_TITLE       (TAG_TR+1)
#define TAG_UL          (TAG_TITLE+1)
#define TAG_XML         (TAG_UL+1)
#define LAST_TAG        (TAG_XML+1)

#define CLOSING_TAG     0
#define OPENING_TAG     1

struct html_tags {
    char* name;
    char length;
    int flags;
};

extern HTML_TAGS tagattr[LAST_TAG];

extern LIST* mimecap_list;

#define mimecap_ptr(n) ((MIMECAP_ENTRY*)listnum2listitem(mimecap_list,(long)(n)))

extern MIME_SECT mime_article;
extern MIME_SECT* mime_section;
extern short mime_state;
extern char* multipart_separator;

#define NOT_MIME        0
#define TEXT_MIME       1
#define ISOTEXT_MIME    2
#define MESSAGE_MIME    3
#define MULTIPART_MIME  4
#define IMAGE_MIME      5
#define AUDIO_MIME      6
#define APP_MIME        7
#define UNHANDLED_MIME  8
#define SKIP_MIME       9
#define DECODE_MIME     10
#define BETWEEN_MIME    11
#define END_OF_MIME     12
#define HTMLTEXT_MIME   13
#define ALTERNATE_MIME  14

#define MENCODE_NONE            0
#define MENCODE_BASE64          1
#define MENCODE_QPRINT          2
#define MENCODE_UUE             3
#define MENCODE_UNHANDLED       4

struct mimecap_entry {
    char* contenttype;
    char* command;
    char* testcommand;
    char* label;
    int flags;
};

#define MCF_NEEDSTERMINAL       0x0001
#define MCF_COPIOUSOUTPUT       0x0002

extern bool auto_view_inline;
extern char* mime_getc_line;

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void mime_init (void);
void mime_ReadMimecap (char*);
MIMECAP_ENTRY* mime_FindMimecapEntry (char*,int);
bool mime_TypesMatch (char*,char*);
int mime_Exec (char*);
void mime_InitSections (void);
void mime_PushSection (void);
bool mime_PopSection (void);
void mime_ClearStruct (MIME_SECT*);
void mime_SetArticle (void);
void mime_ParseType (MIME_SECT*,char*);
void mime_ParseDisposition (MIME_SECT*,char*);
void mime_ParseEncoding (MIME_SECT*,char*);
void mime_ParseSubheader (FILE*,char*);
void mime_SetState (char*);
int mime_EndOfSection (char*);
char* mime_ParseParams (char*);
char* mime_FindParam (char*,char*);
char* mime_SkipWhitespace (char*);
void mime_DecodeArticle (bool);
void mime_Description (MIME_SECT*,char*,int);
int qp_decodestring (char*,char*,bool);
int qp_decode (FILE*,int);
int b64_decodestring (char*,char*);
int b64_decode (FILE*,int);
int cat_decode (FILE*,int);
int filter_html (char*,char*);
